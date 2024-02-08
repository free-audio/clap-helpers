#pragma once

#include <clap/clap.h>

#include "checking-level.hh"
#include "misbehaviour-handler.hh"
#include "host.hh"

namespace clap { namespace helpers {
   // clap_plugin_audio_ports
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginAudioPortsProxy {
   public:
      virtual bool canUseAudioPorts() const noexcept = 0;
      virtual uint32_t audioPortsCount(bool isInput) const noexcept = 0;
      virtual bool
      audioPortsGet(uint32_t index, bool isInput, clap_audio_port_info_t *info) const noexcept = 0;
   };

   // clap_plugin_gui
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginGuiProxy {
   public:
      virtual bool canUseGui() const noexcept = 0;
      virtual bool guiIsApiSupported(const char *api, bool isFloating) const noexcept = 0;
      virtual bool guiGetPreferredApi(const char **api, bool *isFloating) const noexcept = 0;
      virtual bool guiCreate(const char *api, bool isFloating) const noexcept = 0;
      virtual void guiDestroy() const noexcept = 0;
      virtual bool guiSetScale(double scale) const noexcept = 0;
      virtual bool guiGetSize(uint32_t *width, uint32_t *height) const noexcept = 0;
      virtual bool guiCanResize() const noexcept = 0;
      virtual bool guiGetResizeHints(clap_gui_resize_hints_t *hints) const noexcept = 0;
      virtual bool guiAdjustSize(uint32_t *width, uint32_t *height) const noexcept = 0;
      virtual bool guiSetSize(uint32_t width, uint32_t height) const noexcept = 0;
      virtual bool guiSetParent(const clap_window_t *window) const noexcept = 0;
      virtual bool guiSetTransient(const clap_window_t *window) const noexcept = 0;
      virtual void guiSuggestTitle(const char *title) const noexcept = 0;
      virtual bool guiShow() const noexcept = 0;
      virtual bool guiHide() const noexcept = 0;
   };

   // clap_plugin_latency
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginLatencyProxy {
   public:
      virtual bool canUseLatency() const noexcept = 0;
      virtual uint32_t latencyGet() const noexcept = 0;
   };

   // clap_plugin_note_ports
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginNotePortsProxy {
   public:
      virtual bool canUseNotePorts() const noexcept = 0;
      virtual uint32_t notePortsCount(bool is_input) const noexcept = 0;
      virtual bool
      notePortsGet(uint32_t index, bool is_input, clap_note_port_info_t *info) const noexcept = 0;
   };

   // clap_plugin_params
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginParamsProxy {
   public:
      virtual bool canUseParams() const noexcept = 0;
      virtual uint32_t paramsCount() const noexcept = 0;
      virtual bool paramsGetInfo(uint32_t paramIndex,
                                 clap_param_info_t *paramInfo) const noexcept = 0;
      virtual bool paramsGetValue(clap_id paramId, double *outValue) const noexcept = 0;
      virtual bool paramsValueToText(clap_id paramId,
                                     double value,
                                     char *outBuffer,
                                     uint32_t outBufferCapacity) const noexcept = 0;
      virtual bool paramsTextToValue(clap_id paramId,
                                     const char *paramValueText,
                                     double *outValue) const noexcept = 0;
      virtual void paramsFlush(const clap_input_events_t *in,
                               const clap_output_events_t *out) const noexcept = 0;
   };

   // clap_plugin_posix_fd_support
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginPosixFdSupportProxy {
   public:
      virtual bool canUsePosixFdSupport() const noexcept = 0;
      virtual void posixFdSupportOnFd(int fd, clap_posix_fd_flags_t flags) const noexcept = 0;
   };

   // clap_plugin_preset_load
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginPresetLoadProxy {
   public:
      virtual bool canUsePresetLoad() const noexcept = 0;
      virtual bool presetLoadFromLocation(uint32_t locationKind,
                                          const char *location,
                                          const char *loadKey) const noexcept = 0;
   };

   // clap_plugin_remote_controls
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginRemoteControlsProxy {
   public:
      virtual bool canUseRemoteControls() const noexcept = 0;
      virtual uint32_t remoteControlsCount() const noexcept = 0;
      virtual bool remoteControlsGet(uint32_t pageIndex,
                                     clap_remote_controls_page_t *page) const noexcept = 0;
   };

   // clap_plugin_render
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginRenderProxy {
   public:
      virtual bool canUseRender() const noexcept = 0;
      virtual bool renderHasHardRealtimeRequirement() const noexcept = 0;
      virtual bool renderSet(clap_plugin_render_mode mode) const noexcept = 0;
   };

   // clap_plugin_state
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginStateProxy {
   public:
      virtual bool canUseState() const noexcept = 0;
      virtual bool stateSave(const clap_ostream_t *stream) const noexcept = 0;
      virtual bool stateLoad(const clap_istream_t *stream) const noexcept = 0;
   };

   // clap_plugin_tail
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginTailProxy {
   public:
      virtual bool canUseTail() const noexcept = 0;
      virtual uint32_t tailGet() const noexcept = 0;
   };

   // clap_plugin_thread_pool
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginThreadPoolProxy {
   public:
      virtual bool canUseThreadPool() const noexcept = 0;
      virtual void threadPoolExec(uint32_t taskIndex) const noexcept = 0;
   };

   // clap_plugin_timer_support
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginTimerSupportProxy {
   public:
      virtual bool canUseTimerSupport() const noexcept = 0;
      virtual void timerSupportOnTimer(clap_id timerId) const noexcept = 0;
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginProxy : public PluginAudioPortsProxy<h, l>,
                       public PluginGuiProxy<h, l>,
                       public PluginLatencyProxy<h, l>,
                       public PluginNotePortsProxy<h, l>,
                       public PluginParamsProxy<h, l>,
                       public PluginPosixFdSupportProxy<h, l>,
                       public PluginPresetLoadProxy<h, l>,
                       public PluginRemoteControlsProxy<h, l>,
                       public PluginRenderProxy<h, l>,
                       public PluginStateProxy<h, l>,
                       public PluginTailProxy<h, l>,
                       public PluginThreadPoolProxy<h, l>,
                       public PluginTimerSupportProxy<h, l> {
   public:
      PluginProxy(const clap_plugin& plugin, const Host<h, l>& host) : _plugin{plugin}, _host{host} {}

      /////////////////
      // clap_plugin //
      /////////////////
      const clap_plugin* clapPlugin() const { return &_plugin; }
      bool init() noexcept;
      template <typename T>
      void getExtension(const T *&ptr, const char *id) const noexcept;
      void destroy() noexcept;
      bool activate(double sampleRate, uint32_t minFramesCount, uint32_t maxFramesCount) noexcept;
      void deactivate() noexcept;
      bool startProcessing() noexcept;
      void stopProcessing() noexcept;
      void reset() const noexcept;
      clap_process_status process(const clap_process_t *process) const noexcept;
      void onMainThread() const noexcept;

      /////////////////////////////
      // clap_plugin_audio_ports //
      /////////////////////////////
      bool canUseAudioPorts() const noexcept override;
      uint32_t audioPortsCount(bool isInput) const noexcept override;
      bool audioPortsGet(uint32_t index,
                         bool isInput,
                         clap_audio_port_info_t *info) const noexcept override;

      /////////////////////
      // clap_plugin_gui //
      /////////////////////
      bool canUseGui() const noexcept override;
      bool guiIsApiSupported(const char *api, bool isFloating) const noexcept override;
      bool guiGetPreferredApi(const char **api, bool *isFloating) const noexcept override;
      bool guiCreate(const char *api, bool isFloating) const noexcept override;
      void guiDestroy() const noexcept override;
      bool guiSetScale(double scale) const noexcept override;
      bool guiGetSize(uint32_t *width, uint32_t *height) const noexcept override;
      bool guiCanResize() const noexcept override;
      bool guiGetResizeHints(clap_gui_resize_hints_t *hints) const noexcept override;
      bool guiAdjustSize(uint32_t *width, uint32_t *height) const noexcept override;
      bool guiSetSize(uint32_t width, uint32_t height) const noexcept override;
      bool guiSetParent(const clap_window_t *window) const noexcept override;
      bool guiSetTransient(const clap_window_t *window) const noexcept override;
      void guiSuggestTitle(const char *title) const noexcept override;
      bool guiShow() const noexcept override;
      bool guiHide() const noexcept override;

      /////////////////////////
      // clap_plugin_latency //
      /////////////////////////
      bool canUseLatency() const noexcept override;
      uint32_t latencyGet() const noexcept override;

      ////////////////////////////
      // clap_plugin_note_ports //
      ////////////////////////////
      bool canUseNotePorts() const noexcept override;
      uint32_t notePortsCount(bool is_input) const noexcept override;
      bool notePortsGet(uint32_t index,
                        bool is_input,
                        clap_note_port_info_t *info) const noexcept override;

      ////////////////////////
      // clap_plugin_params //
      ////////////////////////
      bool canUseParams() const noexcept override;
      uint32_t paramsCount() const noexcept override;
      bool paramsGetInfo(uint32_t paramIndex, clap_param_info_t *paramInfo) const noexcept override;
      bool paramsGetValue(clap_id paramId, double *outValue) const noexcept override;
      bool paramsValueToText(clap_id paramId,
                             double value,
                             char *outBuffer,
                             uint32_t outBufferCapacity) const noexcept override;
      bool paramsTextToValue(clap_id paramId,
                             const char *paramValueText,
                             double *outValue) const noexcept override;
      void paramsFlush(const clap_input_events_t *in,
                       const clap_output_events_t *out) const noexcept override;

      //////////////////////////////////
      // clap_plugin_posix_fd_support //
      //////////////////////////////////
      bool canUsePosixFdSupport() const noexcept override;
      void posixFdSupportOnFd(int fd, clap_posix_fd_flags_t flags) const noexcept override;

      /////////////////////////////
      // clap_plugin_preset_load //
      /////////////////////////////
      bool canUsePresetLoad() const noexcept override;
      bool presetLoadFromLocation(uint32_t locationKind,
                                  const char *location,
                                  const char *loadKey) const noexcept override;

      /////////////////////////////////
      // clap_plugin_remote_controls //
      /////////////////////////////////
      bool canUseRemoteControls() const noexcept override;
      uint32_t remoteControlsCount() const noexcept override;
      bool remoteControlsGet(uint32_t pageIndex,
                             clap_remote_controls_page_t *page) const noexcept override;

      ////////////////////////
      // clap_plugin_render //
      ////////////////////////
      bool canUseRender() const noexcept override;
      bool renderHasHardRealtimeRequirement() const noexcept override;
      bool renderSet(clap_plugin_render_mode mode) const noexcept override;

      ///////////////////////
      // clap_plugin_state //
      ///////////////////////
      bool canUseState() const noexcept override;
      bool stateSave(const clap_ostream_t *stream) const noexcept override;
      bool stateLoad(const clap_istream_t *stream) const noexcept override;

      //////////////////////
      // clap_plugin_tail //
      //////////////////////
      bool canUseTail() const noexcept override;
      uint32_t tailGet() const noexcept override;

      /////////////////////////////
      // clap_plugin_thread_pool //
      /////////////////////////////
      bool canUseThreadPool() const noexcept override;
      void threadPoolExec(uint32_t taskIndex) const noexcept override;

      ///////////////////////////////
      // clap_plugin_timer_support //
      ///////////////////////////////
      bool canUseTimerSupport() const noexcept override;
      void timerSupportOnTimer(clap_id timerId) const noexcept override;

   protected:
      /////////////////////
      // Thread Checking //
      /////////////////////
      void ensureMainThread(const char *method) const noexcept;
      void ensureAudioThread(const char *method) const noexcept;
      void ensureActivated(const char *method, bool expectedState) const noexcept;
      void ensureProcessing(const char *method, bool expectedState) const noexcept;

      const Host<h, l>& _host;

      const clap_plugin& _plugin;

      const clap_plugin_audio_ports *_pluginAudioPorts = nullptr;
      const clap_plugin_gui *_pluginGui = nullptr;
      const clap_plugin_latency *_pluginLatency = nullptr;
      const clap_plugin_note_ports *_pluginNotePorts = nullptr;
      const clap_plugin_params *_pluginParams = nullptr;
      const clap_plugin_posix_fd_support *_pluginPosixFdSupport = nullptr;
      const clap_plugin_preset_load *_pluginPresetLoad = nullptr;
      const clap_plugin_remote_controls *_pluginRemoteControls = nullptr;
      const clap_plugin_render *_pluginRender = nullptr;
      const clap_plugin_state *_pluginState = nullptr;
      const clap_plugin_tail *_pluginTail = nullptr;
      const clap_plugin_thread_pool *_pluginThreadPool = nullptr;
      const clap_plugin_timer_support *_pluginTimerSupport = nullptr;

      // state
      bool _isActive = false;
      bool _isProcessing = false;
   };
}} // namespace clap::helpers
