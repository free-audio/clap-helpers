#pragma once
#include <cassert>
#include <clap/clap.h>

#include "checking-level.hh"
#include "misbehaviour-handler.hh"

namespace clap { namespace helpers {
   template <MisbehaviourHandler h, CheckingLevel l>
   class PluginProxy {
   public:
      PluginProxy(const clap_plugin *plugin);

      /////////////////
      // clap_plugin //
      /////////////////
      bool init() noexcept;
      template <typename T>
      void getExtension(const T *&ptr, const char *id) noexcept;
      void destroy() noexcept;
      bool activate(double sampleRate, uint32_t minFramesCount, uint32_t maxFramesCount) noexcept;
      void deactivate() noexcept;
      bool startProcessing() noexcept;
      void stopProcessing() noexcept;
      void reset() noexcept;
      clap_process_status process(const clap_process_t *process) noexcept;
      void onMainThread() noexcept;

      /////////////////////////////
      // clap_plugin_audio_ports //
      /////////////////////////////
      bool canUsePluginAudioPorts() const noexcept;
      uint32_t audioPortsCount(bool isInput) noexcept;
      bool audioPortsGet(uint32_t index, bool isInput, clap_audio_port_info_t *info) noexcept;

      /////////////////////
      // clap_plugin_gui //
      /////////////////////
      bool canUsePluginGui() const noexcept;
      bool guiIsApiSupported(const char *api, bool isFloating) noexcept;
      bool guiGetPreferredApi(const char **api, bool *isFloating) noexcept;
      bool guiCreate(const char *api, bool isFloating) noexcept;
      void guiDestroy() noexcept;
      bool guiSetScale(double scale) noexcept;
      bool guiGetSize(uint32_t *width, uint32_t *height) noexcept;
      bool guiCanResize() noexcept;
      bool guiGetResizeHints(clap_gui_resize_hints_t *hints) noexcept;
      bool guiAdjustSize(uint32_t *width, uint32_t *height) noexcept;
      bool guiSetSize(uint32_t width, uint32_t height) noexcept;
      bool guiSetParent(const clap_window_t *window) noexcept;
      bool guiSetTransient(const clap_window_t *window) noexcept;
      void guiSuggestTitle(const char *title) noexcept;
      bool guiShow() noexcept;
      bool guiHide();

      ////////////////////////
      // clap_plugin_params //
      ////////////////////////
      bool canUsePluginParams() const noexcept;
      uint32_t paramsCount() noexcept;
      bool paramsGetInfo(uint32_t paramIndex, clap_param_info_t *paramInfo) noexcept;
      bool paramsGetValue(clap_id paramId, double *outValue) noexcept;
      bool paramsValueToText(clap_id paramId,
                             double value,
                             char *outBuffer,
                             uint32_t outBufferCapacity) noexcept;
      bool paramsTextToValue(clap_id paramId,
                             const char *paramValueText,
                             double *outValue) noexcept;
      void paramsFlush(const clap_input_events_t *in, const clap_output_events_t *out) noexcept;

   protected:
      void ensureMainThread(const char *method) const noexcept;
      void ensureAudioThread(const char *method) const noexcept;

      const clap_plugin *const _plugin;

      const clap_plugin_audio_ports *_pluginAudioPorts = nullptr;
      const clap_plugin_fd_support *_pluginFdSupport = nullptr;
      const clap_plugin_gui *_pluginGui = nullptr;
      const clap_plugin_gui_x11 *_pluginGuiX11 = nullptr;
      const clap_plugin_gui_win32 *_pluginGuiWin32 = nullptr;
      const clap_plugin_gui_cocoa *_pluginGuiCocoa = nullptr;
      const clap_plugin_gui_free_standing *_pluginGuiFreeStanding = nullptr;
      const clap_plugin_params *_pluginParams = nullptr;
      const clap_plugin_preset_load *_pluginPresetLoad = nullptr;
      const clap_plugin_remote_controls *_pluginRemoteControls = nullptr;
      const clap_plugin_state *_pluginState = nullptr;
      const clap_plugin_thread_pool *_pluginThreadPool = nullptr;
      const clap_plugin_timer_support *_pluginTimerSupport = nullptr;
   };
}} // namespace clap::helpers
