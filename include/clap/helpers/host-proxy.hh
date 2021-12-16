#pragma once

#include <string>

#include <clap/all.h>

#include "checking-level.hh"
#include "misbehaviour-handler.hh"

namespace clap { namespace helpers {
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostProxy {
   public:
      HostProxy(const clap_host *host);

      void init();

      template <typename T>
      void getExtension(const T *&ptr, const char *id) noexcept;

      ///////////////////
      // clap_host_log //
      ///////////////////
      void log(clap_log_severity severity, const char *msg) const noexcept;
      void hostMisbehaving(const char *msg) const noexcept;
      void hostMisbehaving(const std::string &msg) const noexcept { hostMisbehaving(msg.c_str()); }
      void pluginMisbehaving(const char *msg) const noexcept;
      void pluginMisbehaving(const std::string &msg) const noexcept {
         pluginMisbehaving(msg.c_str());
      }

      ////////////////////////////
      // clap_host_thread_check //
      ////////////////////////////
      bool isMainThread() const noexcept;
      bool isAudioThread() const noexcept;

      //////////////////////////////////
      // clap_host_audio_ports_config //
      //////////////////////////////////
      void audioPortsConfigRescan() const noexcept;

      ///////////////////////////
      // clap_host_audio_ports //
      ///////////////////////////
      uint32_t audioPortsGetPreferredSampleSize() const noexcept;
      void audioPortsRescan(uint32_t flags) const noexcept;

      //////////////////////////
      // clap_host_note_ports //
      //////////////////////////
      void notePortsRescan(uint32_t flags) const noexcept;

      /////////////////////
      // clap_host_state //
      /////////////////////
      void stateMarkDirty() const noexcept;

      ///////////////////////
      // clap_host_latency //
      ///////////////////////
      void latencyChanged() const noexcept;

      ////////////////////////////
      // clap_host_event_filter //
      ////////////////////////////
      void eventFilterChanged() const noexcept;

      /////////////////////////
      // clap_host_note_name //
      /////////////////////////
      void noteNameChanged() const noexcept;

      //////////////////////
      // clap_host_params //
      //////////////////////
      void paramsRescan(clap_param_rescan_flags flags) const noexcept;
      void paramsClear(clap_id param_id, clap_param_clear_flags flags) const noexcept;
      void paramsRequestFlush() const noexcept;

      //////////////////////////
      // clap_host_track_info //
      //////////////////////////
      bool trackInfoGet(clap_track_info *info) const noexcept;

      ///////////////////
      // clap_host_gui //
      ///////////////////
      bool guiResize(uint32_t width, uint32_t height) const noexcept;

      /////////////////////////////
      // clap_host_timer_support //
      /////////////////////////////
      bool timerSupportRegisterTimer(uint32_t period_ms, clap_id *timer_id) const noexcept;
      bool timerSupportUnregisterTimer(clap_id timer_id) const noexcept;

      //////////////////////////
      // clap_host_fd_support //
      //////////////////////////
      bool fdSupportRegisterFD(clap_fd fd, clap_fd_flags flags) const noexcept;
      bool fdSupportModifyFD(clap_fd fd, clap_fd_flags flags) const noexcept;
      bool fdSupportUnregisterFD(clap_fd fd) const noexcept;

      /////////////////////////////////
      // Interface consistency check //
      /////////////////////////////////
      bool canUseHostLog() const noexcept;
      bool canUseThreadCheck() const noexcept;
      bool canUseTrackInfo() const noexcept;
      bool canUseState() const noexcept;
      bool canUseTimerSupport() const noexcept;
      bool canUseFdSupport() const noexcept;
      bool canUseParams() const noexcept;
      bool canUseLatency() const noexcept;
      bool canUseQuickControls() const noexcept;
      bool canUseGui() const noexcept;
      bool canUseAudioPorts() const noexcept;
      bool canUseAudioPortsConfig() const noexcept;
      bool canUseNotePorts() const noexcept;
      bool canUseEventFilter() const noexcept;
      bool canUseNoteName() const noexcept;

   protected:
      void ensureMainThread(const char *method) const noexcept;
      void ensureAudioThread(const char *method) const noexcept;

      const clap_host *const _host;

      const clap_host_log *_hostLog = nullptr;
      const clap_host_thread_check *_hostThreadCheck = nullptr;
      const clap_host_thread_pool *_hostThreadPool = nullptr;
      const clap_host_audio_ports *_hostAudioPorts = nullptr;
      const clap_host_audio_ports_config *_hostAudioPortsConfig = nullptr;
      const clap_host_note_ports *_hostNotePorts = nullptr;
      const clap_host_event_filter *_hostEventFilter = nullptr;
      const clap_host_file_reference *_hostFileReference = nullptr;
      const clap_host_latency *_hostLatency = nullptr;
      const clap_host_gui *_hostGui = nullptr;
      const clap_host_timer_support *_hostTimerSupport = nullptr;
      const clap_host_fd_support *_hostFdSupport = nullptr;
      const clap_host_params *_hostParams = nullptr;
      const clap_host_track_info *_hostTrackInfo = nullptr;
      const clap_host_state *_hostState = nullptr;
      const clap_host_note_name *_hostNoteName = nullptr;
      const clap_host_quick_controls *_hostQuickControls = nullptr;
   };
}} // namespace clap::helpers