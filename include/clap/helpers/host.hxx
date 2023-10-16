#include <cstring>
#include <iostream>

#include "host.hh"

namespace clap { namespace helpers {

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_audio_ports Host<h, l>::_hostAudioPorts = {
      clapAudioPortsIsRescanFlagSupported,
      clapAudioPortsRescan,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_gui Host<h, l>::_hostGui = {
      clapGuiResizeHintsChanged,
      clapGuiRequestResize,
      clapGuiRequestShow,
      clapGuiRequestHide,
      clapGuiClosed,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_latency Host<h, l>::_hostLatency = {
      clapLatencyChanged,
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
   const clap_host_posix_fd_support Host<h, l>::_hostPosixFdSupport = {
      clapPosixFdSupportRegisterFd,
      clapPosixFdSupportModifyFd,
      clapPosixFdSupportUnregisterFd,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_remote_controls Host<h, l>::_hostRemoteControls = {
      clapRemoteControlsChanged,
      clapRemoteControlsSuggestPage,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_state Host<h, l>::_hostState = {
      clapStateMarkDirty,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_timer_support Host<h, l>::_hostTimerSupport = {
      clapTimerSupportRegisterTimer,
      clapTimerSupportUnregisterTimer,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_tail Host<h, l>::_hostTail = {
      clapTailChanged,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_thread_check Host<h, l>::_hostThreadCheck = {
      clapThreadCheckIsMainThread,
      clapThreadCheckIsAudioThread,
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   const clap_host_thread_pool Host<h, l>::_hostThreadPool = {
      clapThreadPoolRequestExec,
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
   const void *Host<h, l>::clapGetExtension(const clap_host_t *host,
                                            const char *extension_id) noexcept {
      auto &self = from(host);
      if (!std::strcmp(extension_id, CLAP_EXT_AUDIO_PORTS) && self.implementsAudioPorts())
         return &_hostAudioPorts;
      if (!std::strcmp(extension_id, CLAP_EXT_GUI) && self.implementsGui())
         return &_hostGui;
      if (!std::strcmp(extension_id, CLAP_EXT_LATENCY) && self.implementsLatency())
         return &_hostLatency;
      if (!std::strcmp(extension_id, CLAP_EXT_LOG) && self.implementsLog())
         return &_hostLog;
      if (!std::strcmp(extension_id, CLAP_EXT_PARAMS) && self.implementsParams())
         return &_hostParams;
      if (!strcmp(extension_id, CLAP_EXT_POSIX_FD_SUPPORT) && self.implementsPosixFdSupport())
         return &_hostPosixFdSupport;
      if (!strcmp(extension_id, CLAP_EXT_REMOTE_CONTROLS) && self.implementsRemoteControls())
         return &_hostRemoteControls;
      if (!strcmp(extension_id, CLAP_EXT_STATE) && self.implementsState())
         return &_hostState;
      if (!strcmp(extension_id, CLAP_EXT_TIMER_SUPPORT) && self.implementsTimerSupport())
         return &_hostTimerSupport;
      if (!std::strcmp(extension_id, CLAP_EXT_TAIL) && self.implementsTail())
         return &_hostTail;
      if (!std::strcmp(extension_id, CLAP_EXT_THREAD_CHECK))
         return &_hostThreadCheck;
      if (!strcmp(extension_id, CLAP_EXT_THREAD_POOL) && self.implementsThreadPool())
         return &_hostThreadPool;
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
   bool Host<h, l>::clapAudioPortsIsRescanFlagSupported(const clap_host_t *host,
                                                        uint32_t flag) noexcept {
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

   //-------------------//
   // clap_host_latency //
   //-------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapLatencyChanged(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.latencyChanged();
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
   void Host<h, l>::clapParamsRescan(const clap_host_t *host,
                                     clap_param_rescan_flags flags) noexcept {
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

   //----------------------------//
   // clap_host_posix_fd_support //
   //----------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapPosixFdSupportRegisterFd(const clap_host *host,
                                                 int fd,
                                                 clap_posix_fd_flags_t flags) noexcept {
      auto &self = from(host);
      return self.posixFdSupportRegisterFd(fd, flags);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapPosixFdSupportModifyFd(const clap_host *host,
                                               int fd,
                                               clap_posix_fd_flags_t flags) noexcept {
      auto &self = from(host);
      return self.posixFdSupportModifyFd(fd, flags);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapPosixFdSupportUnregisterFd(const clap_host *host, int fd) noexcept {
      auto &self = from(host);
      return self.posixFdSupportUnregisterFd(fd);
   }

   //---------------------------//
   // clap_host_remote_controls //
   //---------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapRemoteControlsChanged(const clap_host *host) noexcept {
      auto &self = from(host);
      self.remoteControlsChanged();
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapRemoteControlsSuggestPage(const clap_host *host, clap_id page_id) noexcept {
      auto &self = from(host);
      self.remoteControlsSuggestPage(page_id);
   }

   //---------------- //
   // clap_host_state //
   //-----------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapStateMarkDirty(const clap_host *host) noexcept {
      auto &self = from(host);
      self.stateMarkDirty();
   }

   //----------------//
   // clap_host_tail //
   //----------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   void Host<h, l>::clapTailChanged(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.tailChanged();
   }

   //-------------------------//
   // clap_host_timer_support //
   //-------------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapTimerSupportRegisterTimer(const clap_host *host,
                                                  uint32_t period_ms,
                                                  clap_id *timer_id) noexcept {
      auto &self = from(host);
      return self.timerSupportRegisterTimer(period_ms, timer_id);
   }

   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapTimerSupportUnregisterTimer(const clap_host *host,
                                                    clap_id timer_id) noexcept {
      auto &self = from(host);
      return self.timerSupportUnregisterTimer(timer_id);
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

   //-----------------------//
   // clap_host_thread_pool //
   //-----------------------//
   template <MisbehaviourHandler h, CheckingLevel l>
   bool Host<h, l>::clapThreadPoolRequestExec(const clap_host *host, uint32_t num_tasks) noexcept {
      auto &self = from(host);
      return self.threadPoolRequestExec(num_tasks);
   }

   ///////////////
   // Utilities //
   ///////////////
   template <MisbehaviourHandler h, CheckingLevel l>
   Host<h, l> &Host<h, l>::from(const clap_host *host) noexcept {
      if constexpr (l >= CheckingLevel::Minimal) {
         if (!host) [[unlikely]] {
            std::cerr << "Passed an null host pointer" << std::endl;
            std::terminate();
         }
      }

      auto self = static_cast<Host *>(host->host_data);
      if constexpr (l >= CheckingLevel::Minimal) {
         if (!self) [[unlikely]] {
            std::cerr << "Passed an invalid host pointer because the host_data is null"
                      << std::endl;
            std::terminate();
         }
      }

      return *self;
   }
}} // namespace clap::helpers
