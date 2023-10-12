#include <cstring>
#include <iostream>

#include "host.hh"

namespace clap { namespace helpers {

   const clap_host_audio_ports Host::_hostAudioPorts = {
      clapAudioPortsIsRescanFlagSupported,
      clapAudioPortsRescan,
   };

   const clap_host_gui Host::_hostGui = {
      clapGuiResizeHintsChanged,
      clapGuiRequestResize,
      clapGuiRequestShow,
      clapGuiRequestHide,
      clapGuiClosed,
   };

   const clap_host_latency Host::_hostLatency = {
      clapLatencyChanged,
   };

   const clap_host_log Host::_hostLog = {
      clapLogLog,
   };

   const clap_host_params Host::_hostParams = {
      clapParamsRescan,
      clapParamsClear,
      clapParamsRequestFlush,
   };

   const clap_host_posix_fd_support Host::_hostPosixFdSupport = {
      clapPosixFdSupportRegisterFd,
      clapPosixFdSupportModifyFd,
      clapPosixFdSupportUnregisterFd,
   };

   const clap_host_remote_controls Host::_hostRemoteControls = {
      clapRemoteControlsChanged,
      clapRemoteControlsSuggestPage,
   };

   const clap_host_state Host::_hostState = {
      clapStateMarkDirty,
   };

   const clap_host_timer_support Host::_hostTimerSupport = {
      clapTimerSupportRegisterTimer,
      clapTimerSupportUnregisterTimer,
   };

   const clap_host_tail Host::_hostTail = {
      clapTailChanged,
   };

   const clap_host_thread_check Host::_hostThreadCheck = {
      clapThreadCheckIsMainThread,
      clapThreadCheckIsAudioThread,
   };

   const clap_host_thread_pool Host::_hostThreadPool = {
      clapThreadPoolRequestExec,
   };

   Host::Host(const char *name, const char *vendor, const char *url, const char *version)
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
   const void *Host::clapGetExtension(const clap_host_t *host, const char *extension_id) noexcept {
      auto &self = from(host);
      if (!std::strcmp(extension_id, CLAP_EXT_AUDIO_PORTS) && self.implementsAudioPorts())
         return &_hostAudioPorts;
      if (!std::strcmp(extension, CLAP_EXT_GUI) && self.implementsGui())
         return &_hostGui;
      if (!std::strcmp(extension_id, CLAP_EXT_LATENCY) && self.implementsLatency())
         return &_hostLatency;
      if (!std::strcmp(extension_id, CLAP_EXT_LOG) && self.implementsLog())
         return &_hostLog;
      if (!std::strcmp(extension_id, CLAP_EXT_PARAMS) && self.implementsParams())
         return &_hostParams;
      if (!strcmp(extension, CLAP_EXT_POSIX_FD_SUPPORT) && self.implementsPosixFdSupport())
         return &_hostPosixFdSupport;
      if (!strcmp(extension, CLAP_EXT_REMOTE_CONTROLS) && self.implementsRemoteControls())
         return &_hostRemoteControls;
      if (!strcmp(extension, CLAP_EXT_STATE) && self.implementsHostState())
         return &_hostState;
      if (!strcmp(extension, CLAP_EXT_TIMER_SUPPORT) && self.implementsTimerSupport())
         return &_hostTimerSupport;
      if (!std::strcmp(extension_id, CLAP_EXT_TAIL) && self.implementsTail())
         return &_hostTail;
      if (!std::strcmp(extension_id, CLAP_EXT_THREAD_CHECK) && self.implementsThreadCheck())
         return &_hostThreadCheck;
      if (!strcmp(extension, CLAP_EXT_THREAD_POOL) && self.implementsThreadPool())
         return &_hostThreadPool;
      return nullptr;
   }

   void Host::clapRequestRestart(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.requestRestart();
   }

   void Host::clapRequestProcess(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.requestProcess();
   }

   void Host::clapRequestCallback(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.requestCallback();
   }

   //-----------------------//
   // clap_host_audio_ports //
   //-----------------------//
   bool Host::clapAudioPortsIsRescanFlagSupported(const clap_host_t *host, uint32_t flag) noexcept {
      auto &self = from(host);
      return self.audioPortsIsRescanFlagSupported(flag);
   }

   void Host::clapAudioPortsRescan(const clap_host_t *host, uint32_t flags) noexcept {
      auto &self = from(host);
      self.audioPortsRescan(flags);
   }

   //---------------//
   // clap_host_gui //
   //---------------//
   void Host::clapGuiResizeHintsChanged(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.guiResizeHintsChanged();
   }

   bool Host::clapGuiRequestResize(const clap_host_t *host,
                                   uint32_t width,
                                   uint32_t height) noexcept {
      auto &self = from(host);
      return self.guiRequestResize(width, height);
   }

   bool Host::clapGuiRequestShow(const clap_host_t *host) noexcept {
      auto &self = from(host);
      return self.guiRequestShow();
   }

   bool Host::clapGuiRequestHide(const clap_host_t *host) noexcept {
      auto &self = from(host);
      return self.guiRequestHide();
   }

   void Host::clapGuiClosed(const clap_host_t *host, bool was_destroyed) noexcept {
      auto &self = from(host);
      self.guiClosed(was_destroyed);
   }

   //-------------------//
   // clap_host_latency //
   //-------------------//
   void Host::clapLatencyChanged(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.latencyChanged();
   }

   //---------------//
   // clap_host_log //
   //---------------//
   void Host::clapLogLog(const clap_host_t *host,
                         clap_log_severity severity,
                         const char *message) noexcept {
      auto &self = from(host);
      self.logLog(severity, message);
   }

   //------------------//
   // clap_host_params //
   //------------------//
   void Host::clapParamsRescan(const clap_host_t *host, clap_param_rescan_flags flags) noexcept {
      auto &self = from(host);
      self.paramsRescan(flags);
   }

   void Host::clapParamsClear(const clap_host_t *host,
                              clap_id param_id,
                              clap_param_clear_flags flags) noexcept {
      auto &self = from(host);
      self.paramsClear(param_id, flags);
   }

   void Host::clapParamsRequestFlush(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.paramsRequestFlush();
   }

   //----------------------------//
   // clap_host_posix_fd_support //
   //----------------------------//
   bool Host::clapPosixFdSupportRegisterFd(const clap_host *host, int fd, clap_posix_fd_flags_t flags) noexcept {
      auto &self = from(host);
      return self.posixFdSupportRegisterFd(fd, flags);
   }

   bool Host::clapPosixFdSupportModifyFd(const clap_host *host, int fd, clap_posix_fd_flags_t flags) noexcept {
      auto &self = from(host);
      return self.posixFdSupportModifyFd(fd, flags);
   }

   bool Host::clapPosixFdSupportUnregisterFd(const clap_host *host, int fd) noexcept {
      auto &self = from(host);
      return self.posixFdSupportUnregisterFd(fd);
   }

   //---------------------------//
   // clap_host_remote_controls //
   //---------------------------//
   void Host::clapRemoteControlsChanged(const clap_host *host) noexcept {
      auto &self = from(host);
      self.remoteControlsChanged();
   }

   void Host::clapRemoteControlsSuggestPage(const clap_host *host, clap_id page_id) noexcept {
      auto &self = from(host);
      self.remoteControlsSuggestPage(page_id);
   }

   //---------------- //
   // clap_host_state //
   //-----------------//
   void Host::clapStateMarkDirty(const clap_host *host) noexcept
   {
      auto &self = from(host);
      self.stateMarkDirty();
   }

   //----------------//
   // clap_host_tail //
   //----------------//
   void Host::clapTailChanged(const clap_host_t *host) noexcept {
      auto &self = from(host);
      self.tailChanged();
   }

   //-------------------------//
   // clap_host_timer_support //
   //-------------------------//
   static bool Host::clapTimerSupportRegisterTimer(const clap_host *host, uint32_t period_ms, clap_id *timer_id) noexcept {
      auto &self = from(host);
      return self.timerSupportRegisterTimer(period_ms, timer_id);
   }

   static bool Host::clapTimerSupportUnregisterTimer(const clap_host *host, clap_id timer_id) noexcept {
      auto &self = from(host);
      return self.timerSupportUnregisterTimer(timer_id);
   }

   //------------------------//
   // clap_host_thread_check //
   //------------------------//
   bool Host::clapThreadCheckIsMainThread(const clap_host_t *host) noexcept {
      auto &self = from(host);
      return self.threadCheckIsMainThread();
   }

   bool Host::clapThreadCheckIsAudioThread(const clap_host_t *host) noexcept {
      auto &self = from(host);
      return self.threadCheckIsAudioThread();
   }

   //-----------------------//
   // clap_host_thread_pool //
   //-----------------------//
   bool Host::clapThreadPoolRequestExec(const clap_host *host, uint32_t num_tasks) noexcept {
      auto &self = from(host);
      return self.threadPoolRequestExec(num_tasks);
   }

   ///////////////
   // Utilities //
   ///////////////
   Host &Host::from(const clap_host *host) noexcept {
      if (!host)
         throw std::invalid_argument("Passed a null host pointer");

      auto h = static_cast<Host *>(host->host_data);
      if (!h)
         throw std::invalid_argument(
            "Passed an invalid host pointer because the host_data is null");

      return *h;
   }
}} // namespace clap::helpers
