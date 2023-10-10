#include <cassert>

#include "plugin-proxy.hh"

namespace clap { namespace helpers {
   template <MisbehaviourHandler h, CheckingLevel l>
   PluginProxy<h, l>::PluginProxy(const clap_plugin *plugin) : _plugin{plugin} {}

   /////////////////
   // clap_plugin //
   /////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::init() noexcept {
      if (!_plugin->init(_plugin))
         return false;

      getExtension(_pluginAudioPorts, CLAP_EXT_AUDIO_PORTS);
      getExtension(_pluginGui, CLAP_EXT_GUI);
      getExtension(_pluginParams, CLAP_EXT_PARAMS);
      return true;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   template <typename T>
   void PluginProxy<h, l>::getExtension(const T *&ptr, const char *id) noexcept {
      assert(!ptr);
      assert(id);

      if (_plugin->get_extension)
         ptr = static_cast<const T *>(_plugin->get_extension(_plugin, id));
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::destroy() noexcept { _plugin->destroy(_plugin); }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::activate(double sampleRate,
                              uint32_t minFramesCount,
                              uint32_t maxFramesCount) noexcept {
      return _plugin->activate(_plugin, sampleRate, minFramesCount, maxFramesCount);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::deactivate() noexcept { _plugin->deactivate(_plugin); }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::startProcessing() noexcept { return _plugin->start_processing(_plugin); }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::stopProcessing() noexcept { _plugin->stop_processing(_plugin); }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::reset() noexcept { _plugin->reset(_plugin); }

   template <MisbehaviourHandler h, CheckingLevel l>
   clap_process_status PluginProxy<h, l>::process(const clap_process_t *process) noexcept {
      return _plugin->process(_plugin, process);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::onMainThread() noexcept { _plugin->on_main_thread(_plugin); }

   /////////////////////////////
   // clap_plugin_audio_ports //
   /////////////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUsePluginAudioPorts() const noexcept {
      if (!_pluginAudioPorts)
         return false;

      if (_pluginAudioPorts->count && _pluginAudioPorts->get)
         return true;

      //pluginMisbehaving("clap_plugin_audio_ports is partially implemented");
      return false;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   uint32_t PluginProxy<h, l>::audioPortsCount(bool isInput) noexcept {
      assert(canUsePluginAudioPorts());
      ensureMainThread("audio_ports.count");
      return _pluginAudioPorts->count(_plugin, isInput);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::audioPortsGet(uint32_t index,
                                   bool isInput,
                                   clap_audio_port_info_t *info) noexcept {
      assert(canUsePluginAudioPorts());
      ensureMainThread("audio_ports.get");
      return _pluginAudioPorts->get(_plugin, index, isInput, info);
   }

   /////////////////////
   // clap_plugin_gui //
   /////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUsePluginGui() const noexcept {
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
   bool PluginProxy<h, l>::guiIsApiSupported(const char *api, bool isFloating) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.is_api_supported");
      return _pluginGui->is_api_supported(_plugin, api, isFloating);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiGetPreferredApi(const char **api, bool *isFloating) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.get_preferred_api");
      return _pluginGui->get_preferred_api(_plugin, api, isFloating);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiCreate(const char *api, bool isFloating) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.create");
      return _pluginGui->create(_plugin, api, isFloating);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::guiDestroy() noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.destroy");
      _pluginGui->destroy(_plugin);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiSetScale(double scale) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.set_scale");
      return _pluginGui->set_scale(_plugin, scale);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiGetSize(uint32_t *width, uint32_t *height) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.get_size");
      return _pluginGui->get_size(_plugin, width, height);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiCanResize() noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.can_resize");
      return _pluginGui->can_resize(_plugin);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiGetResizeHints(clap_gui_resize_hints_t *hints) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.get_resize_hints");
      return _pluginGui->get_resize_hints(_plugin, hints);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiAdjustSize(uint32_t *width, uint32_t *height) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.adjust_size");
      return _pluginGui->adjust_size(_plugin, width, height);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiSetSize(uint32_t width, uint32_t height) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.set_size");
      return _pluginGui->set_size(_plugin, width, height);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiSetParent(const clap_window_t *window) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.set_parent");
      return _pluginGui->set_parent(_plugin, window);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiSetTransient(const clap_window_t *window) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.set_transient");
      return _pluginGui->set_transient(_plugin, window);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::guiSuggestTitle(const char *title) noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.suggest_title");
      _pluginGui->suggest_title(_plugin, title);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiShow() noexcept {
      assert(canUsePluginGui());
      ensureMainThread("gui.show");
      return _pluginGui->show(_plugin);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::guiHide() {
      assert(canUsePluginGui());
      ensureMainThread("gui.hide");
      return _pluginGui->hide(_plugin);
   }

   ////////////////////////
   // clap_plugin_params //
   ////////////////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::canUsePluginParams() const noexcept {
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
   uint32_t PluginProxy<h, l>::paramsCount() noexcept {
      assert(canUsePluginParams());
      ensureMainThread("params.count");
      return _pluginParams->count(_plugin);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::paramsGetInfo(uint32_t paramIndex, clap_param_info_t *paramInfo) noexcept {
      assert(canUsePluginParams());
      ensureMainThread("params.get_info");
      return _pluginParams->get_info(_plugin, paramIndex, paramInfo);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::paramsGetValue(clap_id paramId, double *outValue) noexcept {
      assert(canUsePluginParams());
      ensureMainThread("params.get_value");
      return _pluginParams->get_value(_plugin, paramId, outValue);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::paramsValueToText(clap_id paramId,
                                       double value,
                                       char *outBuffer,
                                       uint32_t outBufferCapacity) noexcept {
      assert(canUsePluginParams());
      ensureMainThread("params.value_to_text");
      return _pluginParams->value_to_text(_plugin, paramId, value, outBuffer, outBufferCapacity);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool PluginProxy<h, l>::paramsTextToValue(clap_id paramId,
                                       const char *paramValueText,
                                       double *outValue) noexcept {
      assert(canUsePluginParams());
      ensureMainThread("params.text_to_value");
      return _pluginParams->text_to_value(_plugin, paramId, paramValueText, outValue);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void PluginProxy<h, l>::paramsFlush(const clap_input_events_t *in,
                                 const clap_output_events_t *out) noexcept {
      assert(canUsePluginParams());
      assert(false); // TODO [active ? audio-thread : main-thread]
      _pluginParams->flush(_plugin, in, out);
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
