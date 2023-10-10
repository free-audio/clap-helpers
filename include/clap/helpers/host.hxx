#include <cstring>
#include <iostream>

#include "host.hh"

namespace clap { namespace helpers {

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_gui Host<h, l>::_hostGui = {
      clapGuiResizeHintsChanged,
      clapGuiRequestResize,
      clapGuiRequestShow,
      clapGuiRequestHide,
      clapGuiClosed,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_log Host<h, l>::_hostLog = {
      clapLogLog,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_params Host<h, l>::_hostParams = {
      clapParamsRescan,
      clapParamsClear,
      clapParamsRequestFlush,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_thread_check Host<h, l>::_hostThreadCheck = {
      clapThreadCheckIsMainThread,
      clapThreadCheckIsAudioThread,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   Host<h, l>::Host(const char *name, const char *vendor, const char *url, const char *version)
      : _host{
           CLAP_VERSION,
           this,
           name,
           vendor,
           url,
           version,
           clapGetExtension,
           clapRequestRestart,
           clapRequestProcess,
           clapRequestCallback,
        } {}

   /////////////////////
   // CLAP Interfaces //
   /////////////////////

   //-----------//
   // clap_host //
   //-----------//
   template <MisbehaviourHandler h, CheckingLevel l>
   const void *Host<h, l>::clapGetExtension(const clap_host_t *host, const char *extension_id) noexcept {
      auto &self = from(host);
      if (!std::strcmp(extension_id, CLAP_EXT_LOG) && self.implementsLog())
         return &_hostLog;
      if (!std::strcmp(extension_id, CLAP_EXT_PARAMS) && self.implementsParams())
         return &_hostParams;
      if (!std::strcmp(extension_id, CLAP_EXT_THREAD_CHECK) && self.implementsThreadCheck())
         return &_hostThreadCheck;
      return nullptr;
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapRequestRestart(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.requestRestart();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapRequestProcess(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.requestProcess();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapRequestCallback(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.requestCallback();
   }

   //-----------------------//
   // clap_host_audio_ports //
   //-----------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapAudioPortsIsRescanFlagSupported(const clap_host_t *host, uint32_t flag) noexcept {
      auto &self = from(host);
      return self.audioPortsIsRescanFlagSupported(flag);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapAudioPortsRescan(const clap_host_t *host, uint32_t flags) noexcept {
      auto &self = from(host);
      self.audioPortsRescan(flags);
   }

   //---------------//
   // clap_host_gui //
   //---------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapGuiResizeHintsChanged(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.guiResizeHintsChanged();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapGuiRequestResize(const clap_host_t *host,
                                   uint32_t width,
                                   uint32_t height) noexcept {
      auto &self = from(host);
      return self.guiRequestResize(width, height);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapGuiRequestShow(const clap_host_t *host) noexcept {
      auto &self = from(host);
      return self.guiRequestShow();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapGuiRequestHide(const clap_host_t *host) noexcept {
      auto &self = from(host);
      return self.guiRequestHide();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapGuiClosed(const clap_host_t *host, bool was_destroyed) noexcept {
      auto &self = from(host);
      self.guiClosed(was_destroyed);
   }

   //---------------//
   // clap_host_log //
   //---------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapLogLog(const clap_host_t *host,
                         clap_log_severity severity,
                         const char *message) noexcept {
      auto &self = from(host);
      self.logLog(severity, message);
   }

   //------------------//
   // clap_host_params //
   //------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapParamsRescan(const clap_host_t *host, clap_param_rescan_flags flags) noexcept {
      auto &self = from(host);
      self.paramsRescan(flags);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapParamsClear(const clap_host_t *host,
                              clap_id param_id,
                              clap_param_clear_flags flags) noexcept {
      auto &self = from(host);
      self.paramsClear(param_id, flags);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapParamsRequestFlush(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.paramsRequestFlush();
   }

   //------------------------//
   // clap_host_thread_check //
   //------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapThreadCheckIsMainThread(const clap_host_t *host) noexcept {
      auto &self = from(host);
      return self.threadCheckIsMainThread();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapThreadCheckIsAudioThread(const clap_host_t *host) noexcept {
      auto &self = from(host);
      return self.threadCheckIsAudioThread();
   }

   ///////////////
   // Utilities //
   ///////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   Host<h, l> &Host<h, l>::from(const clap_host *host) noexcept {
      if (!host) {
         std::cerr << "called with a null clap_host pointer!" << std::endl;
         std::terminate();
      }

      if (!host->host_data) {
         std::cerr << "called with a null clap_host->host_data pointer! The plugin must never "
                      "change this pointer!"
                   << std::endl;
         std::terminate();
      }

      return *static_cast<Host *>(host->host_data);
   }
}} // namespace clap::helpers
