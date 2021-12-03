#include <cassert>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <utility>

#include "plugin.hh"

namespace clap { namespace helpers {

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_render Plugin<h, l>::_pluginRender = {
      clapRenderSetMode,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_thread_pool Plugin<h, l>::_pluginThreadPool = {
      clapThreadPoolExec,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_state Plugin<h, l>::_pluginState = {
      clapStateSave,
      clapStateLoad,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_preset_load Plugin<h, l>::_pluginPresetLoad = {
      clapPresetLoadFromFile,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_track_info Plugin<h, l>::_pluginTrackInfo = {
      clapTrackInfoChanged,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_audio_ports Plugin<h, l>::_pluginAudioPorts = {clapAudioPortsCount,
                                                                    clapAudioPortsInfo};

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_audio_ports_config Plugin<h, l>::_pluginAudioPortsConfig = {
      clapAudioPortsConfigCount,
      clapAudioPortsGetConfig,
      clapAudioPortsSetConfig,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_params Plugin<h, l>::_pluginParams = {
      clapParamsCount,
      clapParamsInfo,
      clapParamsValue,
      clapParamsValueToText,
      clapParamsTextToValue,
      clapParamsFlush,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_quick_controls Plugin<h, l>::_pluginQuickControls = {
      clapQuickControlsPageCount,
      clapQuickControlsPageInfo,
      clapQuickControlsSelectPage,
      clapQuickControlsSelectedPage,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_latency Plugin<h, l>::_pluginLatency = {
      clapLatencyGet,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_note_name Plugin<h, l>::_pluginNoteName = {
      clapNoteNameCount,
      clapNoteNameGet,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_timer_support Plugin<h, l>::_pluginTimerSupport = {clapOnTimer};

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_fd_support Plugin<h, l>::_pluginFdSupport = {
      clapOnFd,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_gui Plugin<h, l>::_pluginGui = {
      clapGuiCreate,
      clapGuiDestroy,
      clapGuiSetScale,
      clapGuiSize,
      clapGuiCanResize,
      clapGuiRoundSize,
      clapGuiSetSize,
      clapGuiShow,
      clapGuiHide,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_gui_x11 Plugin<h, l>::_pluginGuiX11 = {
      clapGuiX11Attach,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_gui_win32 Plugin<h, l>::_pluginGuiWin32 = {
      clapGuiWin32Attach,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_gui_cocoa Plugin<h, l>::_pluginGuiCocoa = {
      clapGuiCocoaAttach,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_plugin_gui_free_standing Plugin<h, l>::_pluginGuiFreeStanding = {
      clapGuiFreeStandingOpen,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   Plugin<h, l>::Plugin(const clap_plugin_descriptor *desc, const clap_host *host) : _host(host) {
      _plugin.plugin_data = this;
      _plugin.desc = desc;
      _plugin.init = Plugin<h, l>::clapInit;
      _plugin.destroy = Plugin<h, l>::clapDestroy;
      _plugin.get_extension = nullptr;
      _plugin.process = nullptr;
      _plugin.activate = nullptr;
      _plugin.deactivate = nullptr;
      _plugin.start_processing = nullptr;
      _plugin.stop_processing = nullptr;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   Plugin<h, l>::~Plugin() = default;

   /////////////////////
   // CLAP Interfaces //
   /////////////////////

   //-------------//
   // clap_plugin //
   //-------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapInit(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);

      self._plugin.get_extension = Plugin<h, l>::clapExtension;
      self._plugin.process = Plugin<h, l>::clapProcess;
      self._plugin.activate = Plugin<h, l>::clapActivate;
      self._plugin.deactivate = Plugin<h, l>::clapDeactivate;
      self._plugin.start_processing = Plugin<h, l>::clapStartProcessing;
      self._plugin.stop_processing = Plugin<h, l>::clapStopProcessing;
      self._plugin.on_main_thread = Plugin<h, l>::clapOnMainThread;

      self.initInterfaces();
      self.ensureMainThread("clap_plugin.init");
      return self.init();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapDestroy(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.destroy");
      delete &from(plugin);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapOnMainThread(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.on_main_thread");
      self.onMainThread();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapActivate(const clap_plugin *plugin,
                                   double sample_rate,
                                   uint32_t minFrameCount,
                                   uint32_t maxFrameCount) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.activate");

      if (l >= CheckingLevel::Minimal) {
         if (self._isActive) {
            self.hostMisbehaving("Plugin was activated twice");

            if (sample_rate != self._sampleRate) {
               std::ostringstream msg;
               msg << "The plugin was activated twice and with different sample rates: "
                   << self._sampleRate << " and " << sample_rate
                   << ". The host must deactivate the plugin first." << std::endl
                   << "Simulating deactivation.";
               self.hostMisbehaving(msg.str());
               clapDeactivate(plugin);
            }
         }

         if (sample_rate <= 0) {
            std::ostringstream msg;
            msg << "The plugin was activated with an invalid sample rates: " << sample_rate;
            self.hostMisbehaving(msg.str());
            return false;
         }

         if (minFrameCount < 1) {
            std::ostringstream msg;
            msg << "The plugin was activated with an invalid minimum frame count: "
                << minFrameCount;
            self.hostMisbehaving(msg.str());
            return false;
         }

         if (maxFrameCount > INT32_MAX) {
            std::ostringstream msg;
            msg << "The plugin was activated with an invalid maximum frame count: "
                << maxFrameCount;
            self.hostMisbehaving(msg.str());
            return false;
         }

         if (minFrameCount > maxFrameCount) {
            std::ostringstream msg;
            msg << "The plugin was activated with an invalid minmum and maximum frame count: min > "
                   "max: "
                << minFrameCount << " > " << maxFrameCount;
            self.hostMisbehaving(msg.str());
            return false;
         }
      }

      assert(!self._isActive);
      assert(self._sampleRate == 0);

      if (!self.activate(sample_rate, minFrameCount, maxFrameCount)) {
         assert(!self._isActive);
         assert(self._sampleRate == 0);
         return false;
      }

      self._isActive = true;
      self._sampleRate = sample_rate;
      return true;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapDeactivate(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.deactivate");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isActive) {
            self.hostMisbehaving("The plugin was deactivated twice.");
            return;
         }
      }

      self.deactivate();
      self._isActive = false;
      self._sampleRate = 0;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapStartProcessing(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureAudioThread("clap_plugin.start_processing");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isActive) {
            self.hostMisbehaving(
               "Host called clap_plugin.start_processing() on a deactivated plugin");
            return false;
         }

         if (self._isProcessing) {
            self.hostMisbehaving("Host called clap_plugin.start_processing() twice");
            return true;
         }
      }

      self._isProcessing = self.startProcessing();
      return self._isProcessing;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapStopProcessing(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureAudioThread("clap_plugin.stop_processing");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isActive) {
            self.hostMisbehaving(
               "Host called clap_plugin.stop_processing() on a deactivated plugin");
            return;
         }

         if (!self._isProcessing) {
            self.hostMisbehaving("Host called clap_plugin.stop_processing() twice");
            return;
         }
      }

      self.stopProcessing();
      self._isProcessing = false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   clap_process_status Plugin<h, l>::clapProcess(const clap_plugin *plugin,
                                                 const clap_process *process) noexcept {
      auto &self = from(plugin);
      self.ensureAudioThread("clap_plugin.process");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isActive) {
            self.hostMisbehaving("Host called clap_plugin.process() on a deactivated plugin");
            return CLAP_PROCESS_ERROR;
         }

         if (!self._isProcessing) {
            self.hostMisbehaving(
               "Host called clap_plugin.process() without calling clap_plugin.start_processing()");
            return CLAP_PROCESS_ERROR;
         }
      }

      return self.process(process);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   const void *Plugin<h, l>::clapExtension(const clap_plugin *plugin, const char *id) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin.extension");

      if (!strcmp(id, CLAP_EXT_STATE) && self.implementsState())
         return &_pluginState;
      if (!strcmp(id, CLAP_EXT_PRESET_LOAD) && self.implementsPresetLoad())
         return &_pluginPresetLoad;
      if (!strcmp(id, CLAP_EXT_RENDER) && self.implementsRender())
         return &_pluginRender;
      if (!strcmp(id, CLAP_EXT_TRACK_INFO) && self.implementsTrackInfo())
         return &_pluginTrackInfo;
      if (!strcmp(id, CLAP_EXT_LATENCY) && self.implementsLatency())
         return &_pluginLatency;
      if (!strcmp(id, CLAP_EXT_AUDIO_PORTS) && self.implementsAudioPorts())
         return &_pluginAudioPorts;
      if (!strcmp(id, CLAP_EXT_PARAMS) && self.implementsParams())
         return &_pluginParams;
      if (!strcmp(id, CLAP_EXT_QUICK_CONTROLS) && self.implementQuickControls())
         return &_pluginQuickControls;
      if (!strcmp(id, CLAP_EXT_NOTE_NAME) && self.implementsNoteName())
         return &_pluginNoteName;
      if (!strcmp(id, CLAP_EXT_THREAD_POOL) && self.implementsThreadPool())
         return &_pluginThreadPool;
      if (!strcmp(id, CLAP_EXT_TIMER_SUPPORT) && self.implementsTimerSupport())
         return &_pluginTimerSupport;
      if (!strcmp(id, CLAP_EXT_FD_SUPPORT) && self.implementsFdSupport())
         return &_pluginFdSupport;
      if (!strcmp(id, CLAP_EXT_GUI) && self.implementsGui())
         return &_pluginGui;
      if (!strcmp(id, CLAP_EXT_GUI_X11) && self.implementsGuiX11())
         return &_pluginGuiX11;
      if (!strcmp(id, CLAP_EXT_GUI_WIN32) && self.implementsGuiWin32())
         return &_pluginGuiWin32;
      if (!strcmp(id, CLAP_EXT_GUI_COCOA) && self.implementsGuiCocoa())
         return &_pluginGuiCocoa;
      if (!strcmp(id, CLAP_EXT_GUI_FREE_STANDING) && self.implementsGuiFreeStanding())
         return &_pluginGuiFreeStanding;

      return from(plugin).extension(id);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   template <typename T>
   void Plugin<h, l>::initInterface(const T *&ptr, const char *id) noexcept {
      assert(!ptr);
      assert(id);

      if (_host->get_extension)
         ptr = static_cast<const T *>(_host->get_extension(_host, id));
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::initInterfaces() noexcept {
      initInterface(_hostLog, CLAP_EXT_LOG);
      initInterface(_hostThreadCheck, CLAP_EXT_THREAD_CHECK);
      initInterface(_hostThreadPool, CLAP_EXT_THREAD_POOL);
      initInterface(_hostAudioPorts, CLAP_EXT_AUDIO_PORTS);
      initInterface(_hostTimerSupport, CLAP_EXT_TIMER_SUPPORT);
      initInterface(_hostFdSupport, CLAP_EXT_FD_SUPPORT);
      initInterface(_hostEventFilter, CLAP_EXT_EVENT_FILTER);
      initInterface(_hostFileReference, CLAP_EXT_FILE_REFERENCE);
      initInterface(_hostLatency, CLAP_EXT_LATENCY);
      initInterface(_hostGui, CLAP_EXT_GUI);
      initInterface(_hostParams, CLAP_EXT_PARAMS);
      initInterface(_hostTrackInfo, CLAP_EXT_TRACK_INFO);
      initInterface(_hostState, CLAP_EXT_STATE);
      initInterface(_hostNoteName, CLAP_EXT_NOTE_NAME);
      initInterface(_hostQuickControls, CLAP_EXT_QUICK_CONTROLS);
   }

   //-------------------//
   // clap_plugin_state //
   //-------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t Plugin<h, l>::clapLatencyGet(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_latency.get");

      return self.latencyGet();
   }

   //--------------------//
   // clap_plugin_render //
   //--------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapRenderSetMode(const clap_plugin *plugin,
                                        clap_plugin_render_mode mode) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_render.set_mode");

      switch (mode) {
      case CLAP_RENDER_REALTIME:
      case CLAP_RENDER_OFFLINE:
         self.renderSetMode(mode);
         break;

      default: {
         std::ostringstream msg;
         msg << "host called clap_plugin_render.set_mode with an unknown mode : " << mode;
         self.hostMisbehaving(msg.str());
         break;
      }
      }
   }

   //-------------------------//
   // clap_plugin_thread_pool //
   //-------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapThreadPoolExec(const clap_plugin *plugin, uint32_t task_index) noexcept {
      auto &self = from(plugin);

      self.threadPoolExec(task_index);
   }

   //-------------------//
   // clap_plugin_state //
   //-------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapStateSave(const clap_plugin *plugin, clap_ostream *stream) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_state.save");

      return self.stateSave(stream);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapStateLoad(const clap_plugin *plugin, clap_istream *stream) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_state.load");

      return self.stateLoad(stream);
   }

   //-------------------------//
   // clap_plugin_preset_load //
   //-------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapPresetLoadFromFile(const clap_plugin *plugin, const char *path) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_preset_load.from_file");

      if (l >= CheckingLevel::Minimal) {
         if (!path) {
            self.hostMisbehaving("host called clap_plugin_preset_load.from_file with a null path");
            return false;
         }
      }

      // TODO check if the file is readable

      return self.presetLoadFromFile(path);
   }

   //------------------------//
   // clap_plugin_track_info //
   //------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapTrackInfoChanged(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_track_info.changed");

      if (l >= CheckingLevel::Minimal) {
         if (!self.canUseTrackInfo()) {
            self.hostMisbehaving(
               "host called clap_plugin_track_info.changed() but does not provide a "
               "complete clap_host_track_info interface");
            return;
         }
      }

      self.trackInfoChanged();
   }

   //-------------------------//
   // clap_plugin_audio_ports //
   //-------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t Plugin<h, l>::clapAudioPortsCount(const clap_plugin *plugin, bool is_input) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.count");

      return self.audioPortsCount(is_input);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapAudioPortsInfo(const clap_plugin *plugin,
                                         uint32_t index,
                                         bool is_input,
                                         clap_audio_port_info *info) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.info");

      if (l >= CheckingLevel::Minimal) {
         auto count = clapAudioPortsCount(plugin, is_input);
         if (index >= count) {
            std::ostringstream msg;
            msg << "Host called clap_plugin_audio_ports.info() with an index out of bounds: "
                << index << " >= " << count;
            self.hostMisbehaving(msg.str());
            return false;
         }
      }

      return self.audioPortsInfo(index, is_input, info);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t Plugin<h, l>::clapAudioPortsConfigCount(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.config_count");
      return self.audioPortsConfigCount();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapAudioPortsGetConfig(const clap_plugin *plugin,
                                              uint32_t index,
                                              clap_audio_ports_config *config) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.get_config");

      if (l >= CheckingLevel::Minimal) {
         auto count = clapAudioPortsConfigCount(plugin);
         if (index >= count) {
            std::ostringstream msg;
            msg << "called clap_plugin_audio_ports.get_config with an index out of bounds: "
                << index << " >= " << count;
            self.hostMisbehaving(msg.str());
            return false;
         }
      }
      return self.audioPortsGetConfig(index, config);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapAudioPortsSetConfig(const clap_plugin *plugin,
                                              clap_id config_id) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_audio_ports.get_config");

      if (l >= CheckingLevel::Minimal) {
         if (self.isActive())
            self.hostMisbehaving(
               "it is illegal to call clap_audio_ports.set_config if the plugin is active");
      }

      return self.audioPortsSetConfig(config_id);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t Plugin<h, l>::clapParamsCount(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_params.count");

      return self.paramsCount();
   }

   //--------------------//
   // clap_plugin_params //
   //--------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapParamsInfo(const clap_plugin *plugin,
                                     int32_t param_index,
                                     clap_param_info *param_info) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_params.info");

      if (l >= CheckingLevel::Minimal) {
         auto count = clapParamsCount(plugin);
         if (param_index >= count) {
            std::ostringstream msg;
            msg << "called clap_plugin_params.info with an index out of bounds: " << param_index
                << " >= " << count;
            self.hostMisbehaving(msg.str());
            return false;
         }
      }

      return self.paramsInfo(param_index, param_info);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapParamsValue(const clap_plugin *plugin,
                                      clap_id param_id,
                                      double *value) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_params.value");

      if (l >= CheckingLevel::Minimal) {
         if (!self.isValidParamId(param_id)) {
            std::ostringstream msg;
            msg << "clap_plugin_params.value called with invalid param_id: " << param_id;
            self.hostMisbehaving(msg.str());
            return false;
         }
      }

      // TODO extra checks

      return self.paramsValue(param_id, value);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapParamsFlush(const clap_plugin *plugin,
                                      const clap_event_list *input_parameter_changes,
                                      const clap_event_list *output_parameter_changes) noexcept {
      auto &self = from(plugin);
      self.ensureParamThread("clap_plugin_params.flush");

      if (l >= CheckingLevel::Maximal) {
         if (!input_parameter_changes)
            self.hostMisbehaving(
               "clap_plugin_params.flush called with an null input parameter change list");
         else {
            uint32_t N = input_parameter_changes->size(input_parameter_changes);
            for (uint32_t i = 0; i < N; ++i) {
               auto ev = input_parameter_changes->get(input_parameter_changes, i);
               if (!ev) {
                  std::ostringstream msg;
                  msg << "clap_plugin_params.flush called null event inside the input list at "
                         "index: "
                      << i;
                  self.hostMisbehaving(msg.str());
                  continue;
               }

               if (ev->type != CLAP_EVENT_PARAM_VALUE) {
                  self.hostMisbehaving("clap_plugin_params.flush must only contain "
                                       "CLAP_EVENT_PARAM_VALUE event type");
                  continue;
               }

               if (self.canUseThreadCheck() && self._hostThreadCheck->is_main_thread(self._host) && !self.isValidParamId(ev->param_value.param_id)) {
                  std::ostringstream msg;
                  msg << "clap_plugin_params.flush called unknown paramId: "
                      << ev->param_value.param_id;
                  self.hostMisbehaving(msg.str());
                  continue;
               }

               // TODO: check range?
            }
         }

         if (!output_parameter_changes)
            self.hostMisbehaving(
               "clap_plugin_params.flush called with an null output parameter change list");
         else if (output_parameter_changes->size(output_parameter_changes) > 0)
            self.hostMisbehaving(
               "clap_plugin_params.flush called with an non-empty output parameter change list");
      }

      self.paramsFlush(input_parameter_changes, output_parameter_changes);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapParamsValueToText(const clap_plugin *plugin,
                                            clap_id param_id,
                                            double value,
                                            char *display,
                                            uint32_t size) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_params.value_to_text");

      if (l >= CheckingLevel::Minimal) {
         if (!self.isValidParamId(param_id)) {
            std::ostringstream msg;
            msg << "clap_plugin_params.value_to_text called with invalid param_id: " << param_id;
            self.hostMisbehaving(msg.str());
            return false;
         }

         if (!display) {
            self.hostMisbehaving(
               "clap_plugin_params.value_to_text called with a null display pointer");
            return false;
         }

         if (size <= 1) {
            self.hostMisbehaving(
               "clap_plugin_params.value_to_text called with a empty buffer (less "
               "than one character)");
            return false;
         }
      }

      return self.paramsValueToText(param_id, value, display, size);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapParamsTextToValue(const clap_plugin *plugin,
                                            clap_id param_id,
                                            const char *display,
                                            double *value) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_params.text_to_value");

      if (l >= CheckingLevel::Minimal) {
         if (!self.isValidParamId(param_id)) {
            std::ostringstream msg;
            msg << "clap_plugin_params.text_to_value called with invalid param_id: " << param_id;
            self.hostMisbehaving(msg.str());
            return false;
         }

         if (!display) {
            self.hostMisbehaving(
               "clap_plugin_params.text_to_value called with a null display pointer");
            return false;
         }

         if (!value) {
            self.hostMisbehaving(
               "clap_plugin_params.text_to_value called with a null value pointer");
            return false;
         }
      }

      return self.paramsTextToValue(param_id, display, value);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::isValidParamId(clap_id param_id) const noexcept {
      checkMainThread();

      auto count = paramsCount();
      clap_param_info info;
      for (uint32_t i = 0; i < count; ++i) {
         if (!paramsInfo(i, &info))
            // TODO: fatal error?
            continue;

         if (info.id == param_id)
            return true;
      }
      return false;
   }

   //----------------------------//
   // clap_plugin_quick_controls //
   //----------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t Plugin<h, l>::clapQuickControlsPageCount(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_quick_controls.page_count");

      return self.quickControlsPageCount();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapQuickControlsPageInfo(const clap_plugin *plugin,
                                                uint32_t page_index,
                                                clap_quick_controls_page *page) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_quick_controls.page_info");

      if (l >= CheckingLevel::Minimal) {
         uint32_t count = clapQuickControlsPageCount(plugin);
         if (page_index >= count) {
            std::ostringstream msg;
            msg
               << "Host called clap_plugin_quick_controls.page_info() with an index out of bounds: "
               << page_index << " >= " << count;
            self.hostMisbehaving(msg.str());
            return false;
         }
      }

      return self.quickControlsPageInfo(page_index, page);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapQuickControlsSelectPage(const clap_plugin *plugin,
                                                  clap_id page_id) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_quick_controls.select_page");

      return self.quickControlsSelectPage(page_id);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   clap_id Plugin<h, l>::clapQuickControlsSelectedPage(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_quick_controls.selected_page");

      return self.quickControlsSelectedPage();
   }

   //-----------------------//
   // clap_plugin_note_name //
   //-----------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t Plugin<h, l>::clapNoteNameCount(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_note_name.count");
      return self.noteNameCount();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapNoteNameGet(const clap_plugin *plugin,
                                      uint32_t index,
                                      clap_note_name *note_name) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_note_name.get");

      if (l >= CheckingLevel::Minimal) {
         auto count = clapNoteNameCount(plugin);
         if (index >= count) {
            std::ostringstream msg;
            msg << "host called clap_plugin_note_name.get with an index out of bounds: " << index
                << " >= " << count;
            self.hostMisbehaving(msg.str());
            return false;
         }
      }

      return self.noteNameGet(index, note_name);
   }

   //------------------------//
   // clap_plugin_event_loop //
   //------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapOnTimer(const clap_plugin *plugin, clap_id timer_id) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_timer_support.on_timer");

      if (l >= CheckingLevel::Minimal) {
         if (timer_id == CLAP_INVALID_ID) {
            self.hostMisbehaving(
               "Host called clap_plugin_timer_support.on_timer with an invalid timer_id");
            return;
         }
      }

      self.onTimer(timer_id);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapOnFd(const clap_plugin *plugin, clap_fd fd, uint32_t flags) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_event_loop.on_fd");

      self.onFd(fd, flags);
   }

   //-----------------//
   // clap_plugin_gui //
   //-----------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapGuiSize(const clap_plugin *plugin,
                                  uint32_t *width,
                                  uint32_t *height) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.size");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving("clap_plugin_gui.size() was called without a prior call to "
                                 "clap_plugin_gui.create()");
            return false;
         }
      }

      return self.guiSize(width, height);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapGuiCanResize(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.can_resize");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving("clap_plugin_gui.can_resize() was called without a prior call to "
                                 "clap_plugin_gui.create()");
            return false;
         }
      }

      return self.guiCanResize();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapGuiRoundSize(const clap_plugin *plugin,
                                       uint32_t *width,
                                       uint32_t *height) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.round_size");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving("clap_plugin_gui.round_size() was called without a prior call to "
                                 "clap_plugin_gui.create()");
            return;
         }
      }

      self.guiRoundSize(width, height);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapGuiSetSize(const clap_plugin *plugin,
                                     uint32_t width,
                                     uint32_t height) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.set_size");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving("clap_plugin_gui.set_size() was called without a prior call to "
                                 "clap_plugin_gui.create()");
            return false;
         }

         if (!self.guiCanResize()) {
            self.hostMisbehaving(
               "clap_plugin_gui.set_size() was called but the gui is not resizable");
            return false;
         }

         uint32_t testWidth = width;
         uint32_t testHeight = height;
         self.guiRoundSize(&testWidth, &testHeight);

         if (width != testWidth || height != testHeight) {
            std::ostringstream os;
            os << "clap_plugin_gui.set_size() was called with a size which was not adjusted by "
                  "clap_plugin_gui.round_size(): "
               << width << "x" << height << " vs " << testWidth << "x" << testHeight;
            self.hostMisbehaving(os.str());
         }
      }

      return self.guiSetSize(width, height);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapGuiSetScale(const clap_plugin *plugin, double scale) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.set_scale");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving("clap_plugin_gui.set_scale() was called without a prior call to "
                                 "clap_plugin_gui.create()");
            return;
         }
      }

      self.guiSetScale(scale);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapGuiShow(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.show");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving("clap_plugin_gui.show() was called without a prior call to "
                                 "clap_plugin_gui.create()");
            return;
         }

         if (!self._isGuiAttached) {
            self.hostMisbehaving("clap_plugin_gui.show() but the gui was not attached first");
            return;
         }
      }

      self.guiShow();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapGuiHide(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.hide");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving("clap_plugin_gui.hide() was called without a prior call to "
                                 "clap_plugin_gui.create()");
            return;
         }

         if (!self._isGuiAttached) {
            self.hostMisbehaving("clap_plugin_gui.hide() but the gui was not attached first");
            return;
         }
      }

      self.guiHide();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapGuiCreate(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.create");

      if (l >= CheckingLevel::Minimal) {
         if (self._isGuiCreated) {
            self.hostMisbehaving(
               "clap_plugin_gui.create() was called while the plugin gui was already created");
            return true;
         }
      }

      if (!self.guiCreate())
         return false;

      self._isGuiCreated = true;
      self._isGuiAttached = false;
      return true;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::clapGuiDestroy(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui.destroy");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving(
               "clap_plugin_gui.destroy() was called while the plugin gui not created");
            return;
         }
      }

      self.guiDestroy();
      self._isGuiCreated = false;
      self._isGuiAttached = false;
   }

   //---------------------//
   // clap_plugin_gui_x11 //
   //---------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapGuiX11Attach(const clap_plugin *plugin,
                                       const char *display_name,
                                       unsigned long window) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui_x11.attach");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving("clap_plugin_gui_x11.attach() was called without a prior call to "
                                 "clap_plugin_gui.create()");
            return false;
         }

         if (self._isGuiAttached) {
            self.hostMisbehaving("clap_plugin_gui_x11.attach() but the gui was already attached");
            return true;
         }
      }

      if (!self.guiX11Attach(display_name, window))
         return false;

      self._isGuiAttached = true;
      return true;
   }

   //-----------------------//
   // clap_plugin_gui_win32 //
   //-----------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapGuiWin32Attach(const clap_plugin *plugin, clap_hwnd window) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui_win32.attach");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving(
               "clap_plugin_gui_win32.attach() was called without a prior call to "
               "clap_plugin_gui.create()");
            return false;
         }

         if (self._isGuiAttached) {
            self.hostMisbehaving("clap_plugin_gui_win32.attach() but the gui was already attached");
            return true;
         }
      }

      if (!self.guiWin32Attach(window))
         return false;

      self._isGuiAttached = true;
      return true;
   }

   //-----------------------//
   // clap_plugin_gui_cocoa //
   //-----------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapGuiCocoaAttach(const clap_plugin *plugin, void *nsView) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui_cocoa.attach");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving(
               "clap_plugin_gui_cocoa.attach() was called without a prior call to "
               "clap_plugin_gui.create()");
            return false;
         }

         if (self._isGuiAttached) {
            self.hostMisbehaving("clap_plugin_gui_cocoa.attach() but the gui was already attached");
            return true;
         }
      }

      if (!self.guiCocoaAttach(nsView))
         return false;

      self._isGuiAttached = true;
      return true;
   }

   //-------------------------------//
   // clap_plugin_gui_free_standing //
   //-------------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::clapGuiFreeStandingOpen(const clap_plugin *plugin) noexcept {
      auto &self = from(plugin);
      self.ensureMainThread("clap_plugin_gui_free_standing.open");

      if (l >= CheckingLevel::Minimal) {
         if (!self._isGuiCreated) {
            self.hostMisbehaving("clap_plugin_gui_free_standing.open() was called without a prior "
                                 "call to clap_plugin_gui.create()");
            return false;
         }

         if (self._isGuiAttached) {
            self.hostMisbehaving(
               "clap_plugin_gui_free_standing.open() but the gui was already attached");
            return true;
         }
      }

      if (!self.guiFreeStandingOpen())
         return false;

      self._isGuiAttached = true;
      return true;
   }

   /////////////
   // Logging //
   /////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::log(clap_log_severity severity, const char *msg) const noexcept {
      if (canUseHostLog())
         _hostLog->log(_host, severity, msg);
      else
         std::clog << msg << std::endl;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::hostMisbehaving(const char *msg) const noexcept {
      log(CLAP_LOG_HOST_MISBEHAVING, msg);

      if (h == MisbehaviourHandler::Terminate)
         std::terminate();
   }

   /////////////////////////////////
   // Interface consistency check //
   /////////////////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::canUseHostLog() const noexcept {
      return _hostLog && _hostLog->log;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::canUseThreadCheck() const noexcept {
      return _hostThreadCheck && _hostThreadCheck->is_audio_thread &&
             _hostThreadCheck->is_main_thread;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::canUseTimerSupport() const noexcept {
      if (!_hostTimerSupport)
         return false;

      auto &x = *_hostTimerSupport;
      if (x.register_timer && x.unregister_timer)
         return true;

      hostMisbehaving("clap_timer_support is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::canUseFdSupport() const noexcept {
      if (!_hostFdSupport)
         return false;

      auto &x = *_hostFdSupport;
      if (x.modify_fd && x.register_fd && x.unregister_fd)
         return true;

      hostMisbehaving("clap_fd_support is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::canUseParams() const noexcept {
      if (!_hostParams)
         return false;

      if (_hostParams->rescan && _hostParams->clear && _hostParams->request_flush)
         return true;

      hostMisbehaving("clap_host_params is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::canUseLatency() const noexcept {
      if (!_hostLatency)
         return false;

      if (_hostLatency->changed)
         return true;

      hostMisbehaving("clap_host_latency is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::canUseQuickControls() const noexcept {
      if (!_hostQuickControls)
         return false;

      if (_hostQuickControls->changed)
         return true;

      hostMisbehaving("clap_host_quick_controls is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::canUseState() const noexcept {
      if (!_hostState)
         return false;

      if (_hostState->mark_dirty)
         return true;

      hostMisbehaving("clap_host_state is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Plugin<h, l>::canUseTrackInfo() const noexcept {
      if (!_hostTrackInfo)
         return false;

      if (_hostTrackInfo->get)
         return true;

      hostMisbehaving("clap_host_track_info is partially implemented");
      return false;
   }

   /////////////////////
   // Thread Checking //
   /////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::checkMainThread() const noexcept {
      if (l == CheckingLevel::None)
         return;

      if (!_hostThreadCheck || !_hostThreadCheck->is_main_thread ||
          _hostThreadCheck->is_main_thread(_host))
         return;

      std::terminate();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::checkAudioThread() const noexcept {
      if (l == CheckingLevel::None)
         return;

      if (!_hostThreadCheck || !_hostThreadCheck->is_audio_thread ||
          _hostThreadCheck->is_audio_thread(_host))
         return;

      std::terminate();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::checkParamThread() const noexcept {
      if (l == CheckingLevel::None)
         return;

      if (isActive())
         checkAudioThread();
      else
         checkMainThread();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::ensureParamThread(const char *method) const noexcept {
      if (l == CheckingLevel::None)
         return;

      if (isActive())
         ensureAudioThread(method);
      else
         ensureMainThread(method);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::ensureMainThread(const char *method) const noexcept {
      if (l == CheckingLevel::None)
         return;

      if (!_hostThreadCheck || !_hostThreadCheck->is_main_thread ||
          _hostThreadCheck->is_main_thread(_host))
         return;

      std::ostringstream msg;
      msg << "Host called the method " << method
          << "() on wrong thread! It must be called on main thread!";
      hostMisbehaving(msg.str());
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Plugin<h, l>::ensureAudioThread(const char *method) const noexcept {
      if (l == CheckingLevel::None)
         return;

      if (!_hostThreadCheck || !_hostThreadCheck->is_audio_thread ||
          _hostThreadCheck->is_audio_thread(_host))
         return;

      std::ostringstream msg;
      msg << "Host called the method " << method
          << "() on wrong thread! It must be called on audio thread!";
      hostMisbehaving(msg.str());
   }

   ///////////////
   // Utilities //
   ///////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   Plugin<h, l> &Plugin<h, l>::from(const clap_plugin *plugin) noexcept {
      if (l >= CheckingLevel::Minimal) {
         if (!plugin) {
            std::cerr << "called with a null clap_plugin pointer!" << std::endl;
            std::terminate();
         }

         if (!plugin->plugin_data) {
            std::cerr << "called with a null clap_plugin->plugin_data pointer! The host must never "
                         "change this pointer!"
                      << std::endl;
            std::terminate();
         }
      }

      return *static_cast<Plugin *>(plugin->plugin_data);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t Plugin<h, l>::compareAudioPortsInfo(const clap_audio_port_info &a,
                                                const clap_audio_port_info &b) noexcept {
      if (a.sample_size != b.sample_size || a.in_place != b.in_place || a.is_cv != b.is_cv ||
          a.is_main != b.is_main || a.channel_count != b.channel_count ||
          a.channel_map != b.channel_map || a.id != b.id)
         return CLAP_AUDIO_PORTS_RESCAN_ALL;

      if (strncmp(a.name, b.name, sizeof(a.name)))
         return CLAP_AUDIO_PORTS_RESCAN_NAMES;

      return 0;
   }
}} // namespace clap::helpers