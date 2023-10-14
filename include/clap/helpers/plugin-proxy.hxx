#include <cassert>

#include "plugin-proxy.hh"

namespace clap { namespace helpers {
   /////////////////
   // clap_plugin //
   /////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::init() noexcept {
      if (!_plugin.init(&_plugin))
         return false;

      getExtension(_pluginAudioPorts, CLAP_EXT_AUDIO_PORTS);
      getExtension(_pluginGui, CLAP_EXT_GUI);
      getExtension(_pluginParams, CLAP_EXT_PARAMS);
      getExtension(_pluginPosixFdSupport, CLAP_EXT_POSIX_FD_SUPPORT);
      getExtension(_pluginPresetLoad, CLAP_EXT_PRESET_LOAD);
      getExtension(_pluginRemoteControls, CLAP_EXT_REMOTE_CONTROLS);
      getExtension(_pluginState, CLAP_EXT_PLUGIN_STATE);
      getExtension(_pluginThreadPool, CLAP_EXT_THREAD_POOL);
      getExtension(_pluginTimerSupport, CLAP_EXT_TIMER_SUPPORT);
      return true;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   template <typename T>
   void PluginProxy<h, l>::getExtension(const T *&ptr, const char *id) const noexcept {
      assert(!ptr);
      assert(id);

      if (_plugin.get_extension)
         ptr = static_cast<const T *>(_plugin.get_extension(&_plugin, id));
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::destroy() noexcept {
      _plugin.destroy(&_plugin);

      _pluginAudioPorts = nullptr;
      _pluginGui = nullptr;
      _pluginParams = nullptr;
      _pluginPosixFdSupport = nullptr;
      _pluginPresetLoad = nullptr;
      _pluginRemoteControls = nullptr;
      _pluginState = nullptr;
      _pluginThreadPool = nullptr;
      _pluginTimerSupport = nullptr;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::activate(double sampleRate,
                              uint32_t minFramesCount,
                              uint32_t maxFramesCount) const noexcept {
      return _plugin.activate(&_plugin, sampleRate, minFramesCount, maxFramesCount);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::deactivate() const noexcept { _plugin.deactivate(&_plugin); }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::startProcessing() const noexcept { return _plugin.start_processing(&_plugin); }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::stopProcessing() const noexcept { _plugin.stop_processing(&_plugin); }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::reset() const noexcept { _plugin.reset(&_plugin); }

   template <MisbehaviourHandler h, CheckingLevel l>
   clap_process_status PluginProxy<h, l>::process(const clap_process_t *process) const noexcept {
      return _plugin.process(&_plugin, process);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::onMainThread() const noexcept { _plugin.on_main_thread(&_plugin); }

   /////////////////////////////
   // clap_plugin_audio_ports //
   /////////////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUseAudioPorts() const noexcept {
      if (!_pluginAudioPorts)
         return false;

      if (_pluginAudioPorts->count && _pluginAudioPorts->get)
         return true;

      //pluginMisbehaving("clap_plugin_audio_ports is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t PluginProxy<h, l>::audioPortsCount(bool isInput) const noexcept {
      assert(canUseAudioPorts());
      ensureMainThread("audio_ports.count");
      return _pluginAudioPorts->count(&_plugin, isInput);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::audioPortsGet(uint32_t index,
                                   bool isInput,
                                   clap_audio_port_info_t *info) const noexcept {
      assert(canUseAudioPorts());
      ensureMainThread("audio_ports.get");
      return _pluginAudioPorts->get(&_plugin, index, isInput, info);
   }

   /////////////////////
   // clap_plugin_gui //
   /////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUseGui() const noexcept {
      if (!_pluginGui)
         return false;

      if (_pluginGui->is_api_supported && _pluginGui->get_preferred_api && _pluginGui->create &&
          _pluginGui->destroy && _pluginGui->set_scale && _pluginGui->get_size &&
          _pluginGui->can_resize && _pluginGui->get_resize_hints && _pluginGui->adjust_size &&
          _pluginGui->set_size && _pluginGui->set_parent && _pluginGui->set_transient &&
          _pluginGui->suggest_title && _pluginGui->show && _pluginGui->hide)
         return true;

      //pluginMisbehaving("clap_plugin_gui is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiIsApiSupported(const char *api, bool isFloating) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.is_api_supported");
      return _pluginGui->is_api_supported(&_plugin, api, isFloating);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiGetPreferredApi(const char **api, bool *isFloating) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.get_preferred_api");
      return _pluginGui->get_preferred_api(&_plugin, api, isFloating);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiCreate(const char *api, bool isFloating) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.create");
      return _pluginGui->create(&_plugin, api, isFloating);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::guiDestroy() const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.destroy");
      _pluginGui->destroy(&_plugin);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiSetScale(double scale) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.set_scale");
      return _pluginGui->set_scale(&_plugin, scale);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiGetSize(uint32_t *width, uint32_t *height) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.get_size");
      return _pluginGui->get_size(&_plugin, width, height);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiCanResize() const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.can_resize");
      return _pluginGui->can_resize(&_plugin);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiGetResizeHints(clap_gui_resize_hints_t *hints) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.get_resize_hints");
      return _pluginGui->get_resize_hints(&_plugin, hints);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiAdjustSize(uint32_t *width, uint32_t *height) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.adjust_size");
      return _pluginGui->adjust_size(&_plugin, width, height);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiSetSize(uint32_t width, uint32_t height) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.set_size");
      return _pluginGui->set_size(&_plugin, width, height);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiSetParent(const clap_window_t *window) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.set_parent");
      return _pluginGui->set_parent(&_plugin, window);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiSetTransient(const clap_window_t *window) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.set_transient");
      return _pluginGui->set_transient(&_plugin, window);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::guiSuggestTitle(const char *title) const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.suggest_title");
      _pluginGui->suggest_title(&_plugin, title);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiShow() const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.show");
      return _pluginGui->show(&_plugin);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiHide() const noexcept {
      assert(canUseGui());
      ensureMainThread("gui.hide");
      return _pluginGui->hide(&_plugin);
   }

   ////////////////////////
   // clap_plugin_params //
   ////////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUseParams() const noexcept {
      if (!_pluginParams)
         return false;

      if (_pluginParams->count
          && _pluginParams->get_info
          && _pluginParams->get_value
          && _pluginParams->value_to_text
          && _pluginParams->text_to_value
          && _pluginParams->flush)
          return true;

      //pluginMisbehaving("clap_plugin_params is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t PluginProxy<h, l>::paramsCount() const noexcept {
      assert(canUseParams());
      ensureMainThread("params.count");
      return _pluginParams->count(&_plugin);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::paramsGetInfo(uint32_t paramIndex, clap_param_info_t *paramInfo) const noexcept {
      assert(canUseParams());
      ensureMainThread("params.get_info");
      return _pluginParams->get_info(&_plugin, paramIndex, paramInfo);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::paramsGetValue(clap_id paramId, double *outValue) const noexcept {
      assert(canUseParams());
      ensureMainThread("params.get_value");
      return _pluginParams->get_value(&_plugin, paramId, outValue);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::paramsValueToText(clap_id paramId,
                                       double value,
                                       char *outBuffer,
                                       uint32_t outBufferCapacity) const noexcept {
      assert(canUseParams());
      ensureMainThread("params.value_to_text");
      return _pluginParams->value_to_text(&_plugin, paramId, value, outBuffer, outBufferCapacity);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::paramsTextToValue(clap_id paramId,
                                       const char *paramValueText,
                                       double *outValue) const noexcept {
      assert(canUseParams());
      ensureMainThread("params.text_to_value");
      return _pluginParams->text_to_value(&_plugin, paramId, paramValueText, outValue);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::paramsFlush(const clap_input_events_t *in,
                                 const clap_output_events_t *out) const noexcept {
      assert(canUseParams());
      assert(false); // TODO [active ? audio-thread : main-thread]
      _pluginParams->flush(&_plugin, in, out);
   }

   //////////////////////////////////
   // clap_plugin_posix_fd_support //
   //////////////////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUsePosixFdSupport() const noexcept {
      if (!_pluginPosixFdSupport)
         return false;

      if (_pluginPosixFdSupport->on_fd)
          return true;

      //pluginMisbehaving("clap_plugin_posix_fd_support is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::posixFdSupportOnFd(int fd, clap_posix_fd_flags_t flags) const noexcept {
      assert(canUsePosixFdSupport());
      ensureMainThread("posix_fd_support.on_fd");
      _pluginPosixFdSupport->on_fd(&_plugin, fd, flags);
   }

   /////////////////////////////
   // clap_plugin_preset_load //
   /////////////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUsePresetLoad() const noexcept {
      if (!_pluginPresetLoad)
         return false;

      if (_pluginPresetLoad->from_location)
          return true;

      //pluginMisbehaving("clap_plugin_preset_load is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::presetLoadFromLocation(uint32_t locationKind,
                                                  const char *location,
                                                  const char *loadKey) const noexcept {
      assert(canUsePresetLoad());
      ensureMainThread("preset_load.from_location");
      return _pluginPresetLoad->from_location(&_plugin, locationKind, location, loadKey);
   }

   /////////////////////////////////
   // clap_plugin_remote_controls //
   /////////////////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUseRemoteControls() const noexcept {
      if (!_pluginRemoteControls)
         return false;

      if (_pluginRemoteControlsState->count && _pluginRemoteControls->get)
          return true;

      //pluginMisbehaving("clap_plugin_remote_controls is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t PluginProxy<h, l>::remoteControlsCount() const noexcept {
      assert(canUseRemoteControls());
      ensureMainThread("remote_controls.count");
      return _pluginRemoteControlsState->count(&_plugin);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::remoteControlsGet(uint32_t                     pageIndex,
                                             clap_remote_controls_page_t *page) const noexcept {
      assert(canUseRemoteControls());
      ensureMainThread("remote_controls.get");
      return _pluginRemoteControlsState->get(&_plugin, pageIndex, page);
   }

   ///////////////////////
   // clap_plugin_state //
   ///////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUseState() const noexcept {
      if (!_pluginState)
         return false;

      if (_pluginState->save && _pluginState->load)
          return true;

      //pluginMisbehaving("clap_plugin_state is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::stateSave(const clap_ostream *stream) const noexcept {
      assert(canUseState());
      ensureMainThread("state.save");
      return _pluginState->save(&_plugin, taskIndex);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::stateLoad(const clap_istream *stream) const noexcept {
      assert(canUseState());
      ensureMainThread("state.load");
      return _pluginState->load(&_plugin, taskIndex);
   }

   /////////////////////////////
   // clap_plugin_thread_pool //
   /////////////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUseThreadPool() const noexcept {
      if (!_pluginThreadPool)
         return false;

      if (_pluginThreadPool->exec)
          return true;

      //pluginMisbehaving("clap_plugin_thread_pool is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::threadPoolExec(uint32_t taskIndex) const noexcept {
      assert(canUseThreadPool());
      _pluginThreadPool->exec(&_plugin, taskIndex);
   }

   ///////////////////////////////
   // clap_plugin_timer_support //
   ///////////////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUseTimerSupport() const noexcept {
      if (!_pluginTimerSupport)
         return false;

      if (_pluginTimerSupport->on_timer)
          return true;

      //pluginMisbehaving("_pluginTimerSupport is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::timerSupportOnTimer(clap_id timerId) const noexcept {
      assert(canUseTimerSupport());
      ensureMainThread("timer_support.on_timer");
      _pluginTimerSupport->on_timer(&_plugin, timerId);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::ensureMainThread(const char *method) const noexcept {
      if (l == CheckingLevel::None)
         return;

      assert(false); // TODO !main-thread -> hostMisbehaving
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::ensureAudioThread(const char *method) const noexcept {
      if (l == CheckingLevel::None)
         return;

      assert(false); // TODO !audio-thread -> hostMisbehaving
   }
}} // namespace clap::helpers
