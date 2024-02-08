#pragma once

#include <string>

#include <clap/all.h>

#include "checking-level.hh"
#include "misbehaviour-handler.hh"

namespace clap { namespace helpers {
   // clap_host_log
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostLogProxy {
   public:
      virtual bool canUseHostLog() const noexcept = 0;
      virtual void log(clap_log_severity severity, const char *msg) const noexcept = 0;
   };

   // clap_host_thread_check
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostThreadCheckProxy {
   public:
      virtual bool canUseThreadCheck() const noexcept = 0;
      virtual bool isMainThread() const noexcept = 0;
      virtual bool isAudioThread() const noexcept = 0;
   };

   // clap_host_audio_ports_config
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostAudioPortsConfigProxy {
   public:
      virtual bool canUseAudioPortsConfig() const noexcept = 0;
      virtual void audioPortsConfigRescan() const noexcept = 0;
   };

   // clap_host_audio_ports
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostAudioPortsProxy {
   public:
      virtual bool canUseAudioPorts() const noexcept = 0;
      virtual void audioPortsRescan(uint32_t flags) const noexcept = 0;
   };

   // clap_host_note_ports
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostNotePortsProxy {
   public:
      virtual bool canUseNotePorts() const noexcept = 0;
      virtual void notePortsRescan(uint32_t flags) const noexcept = 0;
   };

   // clap_host_state
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostStateProxy {
   public:
      virtual bool canUseState() const noexcept = 0;
      virtual void stateMarkDirty() const noexcept = 0;
   };

   // clap_host_latency
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostLatencyProxy {
   public:
      virtual bool canUseLatency() const noexcept = 0;
      virtual void latencyChanged() const noexcept = 0;
   };

   // clap_host_tail
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostTailProxy {
   public:
      virtual bool canUseTail() const noexcept = 0;
      virtual void tailChanged() const noexcept = 0;
   };

   // clap_host_note_name
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostNoteNameProxy {
   public:
      virtual bool canUseNoteName() const noexcept = 0;
      virtual void noteNameChanged() const noexcept = 0;
   };

   // clap_host_params
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostParamsProxy {
   public:
      virtual bool canUseParams() const noexcept = 0;
      virtual void paramsRescan(clap_param_rescan_flags flags) const noexcept = 0;
      virtual void paramsClear(clap_id param_id, clap_param_clear_flags flags) const noexcept = 0;
      virtual void paramsRequestFlush() const noexcept = 0;
   };

   // clap_host_track_info
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostTrackInfoProxy {
   public:
      virtual bool canUseTrackInfo() const noexcept = 0;
      virtual bool trackInfoGet(clap_track_info *info) const noexcept = 0;
   };

   // clap_host_gui
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostGuiProxy {
   public:
      virtual bool canUseGui() const noexcept = 0;
      virtual void guiResizeHintsChanged() const noexcept = 0;
      virtual bool guiRequestResize(uint32_t width, uint32_t height) const noexcept = 0;
      virtual bool guiRequestShow() const noexcept = 0;
      virtual bool guiRequestHide() const noexcept = 0;
      virtual void guiClosed(bool wasDestroyed) const noexcept = 0;
   };

   // clap_host_timer_support
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostTimerSupportProxy {
   public:
      virtual bool canUseTimerSupport() const noexcept = 0;
      virtual bool timerSupportRegister(uint32_t period_ms, clap_id *timer_id) const noexcept = 0;
      virtual bool timerSupportUnregister(clap_id timer_id) const noexcept = 0;
   };

   // clap_host_fd_support
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostPosixFdSupportProxy {
   public:
      virtual bool canUsePosixFdSupport() const noexcept = 0;
      virtual bool posixFdSupportRegister(int fd, clap_posix_fd_flags_t flags) const noexcept = 0;
      virtual bool posixFdSupportModify(int fd, clap_posix_fd_flags_t flags) const noexcept = 0;
      virtual bool posixFdSupportUnregister(int fd) const noexcept = 0;
   };

   // clap_host_remote_controls
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostRemoteControlsProxy {
   public:
      virtual bool canUseRemoteControls() const noexcept = 0;
      virtual void remoteControlsChanged() const noexcept = 0;
      virtual void remoteControlsSuggestPage(clap_id page_id) const noexcept = 0;
   };

   // clap_host_thread_pool
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostThreadPoolProxy {
   public:
      virtual bool canUseThreadPool() const noexcept = 0;
      virtual bool threadPoolRequestExec(uint32_t numTasks) const noexcept = 0;
   };

   // clap_host_voice_info
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostVoiceInfoProxy {
   public:
      virtual bool canUseVoiceInfo() const noexcept = 0;
      virtual void voiceInfoChanged() const noexcept = 0;
   };

   // clap_host_context_menu
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostContextMenuProxy {
   public:
      virtual bool canUseContextMenu() const noexcept = 0;
      virtual bool
      contextMenuPopulate(const clap_context_menu_target_t *target,
                          const clap_context_menu_builder_t *builder) const noexcept = 0;
      virtual bool contextMenuPerform(const clap_context_menu_target_t *target,
                                      clap_id action_id) const noexcept = 0;
      virtual bool contextMenuCanPopup() const noexcept = 0;
      virtual bool contextMenuPopup(const clap_context_menu_target_t *target,
                                    int32_t screen_index,
                                    int32_t x,
                                    int32_t y) const noexcept = 0;
   };

   // clap_host_preset_load
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostPresetLoadProxy {
   public:
      virtual bool canUsePresetLoad() const noexcept = 0;
      virtual void presetLoadOnError(uint32_t location_kind,
                                     const char *location,
                                     const char *load_key,
                                     int32_t os_error,
                                     const char *msg) const noexcept = 0;
      virtual void presetLoadLoaded(uint32_t location_kind,
                                    const char *location,
                                    const char *load_key) const noexcept = 0;
   };

   // clap_host_resource_directory
   template <MisbehaviourHandler h, CheckingLevel l>
   class HostResourceDirectoryProxy {
   public:
      virtual bool canUseResourceDirectory() const noexcept = 0;
      virtual bool requestDirectory(bool isShared) const noexcept = 0;
      virtual void releaseDirectory(bool isShared) const noexcept = 0;
   };

   template <MisbehaviourHandler h, CheckingLevel l>
   class HostProxy : public HostLogProxy<h, l>,
                     public HostThreadCheckProxy<h, l>,
                     public HostAudioPortsConfigProxy<h, l>,
                     public HostAudioPortsProxy<h, l>,
                     public HostNotePortsProxy<h, l>,
                     public HostStateProxy<h, l>,
                     public HostLatencyProxy<h, l>,
                     public HostTailProxy<h, l>,
                     public HostNoteNameProxy<h, l>,
                     public HostParamsProxy<h, l>,
                     public HostTrackInfoProxy<h, l>,
                     public HostGuiProxy<h, l>,
                     public HostTimerSupportProxy<h, l>,
                     public HostPosixFdSupportProxy<h, l>,
                     public HostRemoteControlsProxy<h, l>,
                     public HostThreadPoolProxy<h, l>,
                     public HostVoiceInfoProxy<h, l>,
                     public HostContextMenuProxy<h, l>,
                     public HostPresetLoadProxy<h, l>,
                     public HostResourceDirectoryProxy<h, l> {
   public:
      HostProxy(const clap_host *host);

      // not copyable, not moveable
      HostProxy(const HostProxy &) = delete;
      HostProxy(HostProxy &&) = delete;
      HostProxy &operator=(const HostProxy &) = delete;
      HostProxy &operator=(HostProxy &&) = delete;

      void init();

      const clap_host *host() const noexcept { return _host; }

      ///////////////
      // clap_host //
      ///////////////
      template <typename T>
      void getExtension(const T *&ptr, const char *id) noexcept;
      void requestCallback() noexcept;
      void requestRestart() noexcept;
      void requestProcess() noexcept;

      ///////////////////
      // clap_host_log //
      ///////////////////
      bool canUseHostLog() const noexcept override;
      void log(clap_log_severity severity, const char *msg) const noexcept override;
      void hostMisbehaving(const char *msg) const noexcept;
      void hostMisbehaving(const std::string &msg) const noexcept { hostMisbehaving(msg.c_str()); }
      void pluginMisbehaving(const char *msg) const noexcept;
      void pluginMisbehaving(const std::string &msg) const noexcept {
         pluginMisbehaving(msg.c_str());
      }

      ////////////////////////////
      // clap_host_thread_check //
      ////////////////////////////
      bool canUseThreadCheck() const noexcept override;
      bool isMainThread() const noexcept override;
      bool isAudioThread() const noexcept override;

      //////////////////////////////////
      // clap_host_audio_ports_config //
      //////////////////////////////////
      bool canUseAudioPortsConfig() const noexcept override;
      void audioPortsConfigRescan() const noexcept override;

      ///////////////////////////
      // clap_host_audio_ports //
      ///////////////////////////
      bool canUseAudioPorts() const noexcept override;
      void audioPortsRescan(uint32_t flags) const noexcept override;

      //////////////////////////
      // clap_host_note_ports //
      //////////////////////////
      bool canUseNotePorts() const noexcept override;
      void notePortsRescan(uint32_t flags) const noexcept override;

      /////////////////////
      // clap_host_state //
      /////////////////////
      bool canUseState() const noexcept override;
      void stateMarkDirty() const noexcept override;

      ///////////////////////
      // clap_host_latency //
      ///////////////////////
      bool canUseLatency() const noexcept override;
      void latencyChanged() const noexcept override;

      ////////////////////
      // clap_host_tail //
      ////////////////////
      bool canUseTail() const noexcept override;
      void tailChanged() const noexcept override;

      /////////////////////////
      // clap_host_note_name //
      /////////////////////////
      bool canUseNoteName() const noexcept override;
      void noteNameChanged() const noexcept override;

      //////////////////////
      // clap_host_params //
      //////////////////////
      bool canUseParams() const noexcept override;
      void paramsRescan(clap_param_rescan_flags flags) const noexcept override;
      void paramsClear(clap_id param_id, clap_param_clear_flags flags) const noexcept override;
      void paramsRequestFlush() const noexcept override;

      //////////////////////////
      // clap_host_track_info //
      //////////////////////////
      bool canUseTrackInfo() const noexcept override;
      bool trackInfoGet(clap_track_info *info) const noexcept override;

      ///////////////////
      // clap_host_gui //
      ///////////////////
      bool canUseGui() const noexcept override;
      void guiResizeHintsChanged() const noexcept override;
      bool guiRequestResize(uint32_t width, uint32_t height) const noexcept override;
      bool guiRequestShow() const noexcept override;
      bool guiRequestHide() const noexcept override;
      void guiClosed(bool wasDestroyed) const noexcept override;

      /////////////////////////////
      // clap_host_timer_support //
      /////////////////////////////
      bool canUseTimerSupport() const noexcept override;
      bool timerSupportRegister(uint32_t period_ms, clap_id *timer_id) const noexcept override;
      bool timerSupportUnregister(clap_id timer_id) const noexcept override;

      //////////////////////////
      // clap_host_fd_support //
      //////////////////////////
      bool canUsePosixFdSupport() const noexcept override;
      bool posixFdSupportRegister(int fd, clap_posix_fd_flags_t flags) const noexcept override;
      bool posixFdSupportModify(int fd, clap_posix_fd_flags_t flags) const noexcept override;
      bool posixFdSupportUnregister(int fd) const noexcept override;

      //////////////////////////////
      // clap_host_remote_controls //
      //////////////////////////////
      bool canUseRemoteControls() const noexcept override;
      void remoteControlsChanged() const noexcept override;
      void remoteControlsSuggestPage(clap_id page_id) const noexcept override;

      ///////////////////////////
      // clap_host_thread_pool //
      ///////////////////////////
      bool canUseThreadPool() const noexcept override;
      bool threadPoolRequestExec(uint32_t numTasks) const noexcept override;

      //////////////////////////
      // clap_host_voice_info //
      //////////////////////////
      bool canUseVoiceInfo() const noexcept override;
      void voiceInfoChanged() const noexcept override;

      ////////////////////////////
      // clap_host_context_menu //
      ////////////////////////////
      bool canUseContextMenu() const noexcept override;
      bool contextMenuPopulate(const clap_context_menu_target_t *target,
                               const clap_context_menu_builder_t *builder) const noexcept override;
      bool contextMenuPerform(const clap_context_menu_target_t *target,
                              clap_id action_id) const noexcept override;
      bool contextMenuCanPopup() const noexcept override;
      bool contextMenuPopup(const clap_context_menu_target_t *target,
                            int32_t screen_index,
                            int32_t x,
                            int32_t y) const noexcept override;

      ///////////////////////////
      // clap_host_preset_load //
      ///////////////////////////
      bool canUsePresetLoad() const noexcept override;
      void presetLoadOnError(uint32_t location_kind,
                             const char *location,
                             const char *load_key,
                             int32_t os_error,
                             const char *msg) const noexcept override;
      void presetLoadLoaded(uint32_t location_kind,
                            const char *location,
                            const char *load_key) const noexcept override;

      //////////////////////////////////
      // clap_host_resource_directory //
      //////////////////////////////////
      bool canUseResourceDirectory() const noexcept override;
      bool requestDirectory(bool isShared) const noexcept override;
      void releaseDirectory(bool isShared) const noexcept override;

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
      const clap_host_resource_directory *_hostResourceDirectory = nullptr;
      const clap_host_latency *_hostLatency = nullptr;
      const clap_host_gui *_hostGui = nullptr;
      const clap_host_timer_support *_hostTimerSupport = nullptr;
      const clap_host_posix_fd_support *_hostPosixFdSupport = nullptr;
      const clap_host_params *_hostParams = nullptr;
      const clap_host_track_info *_hostTrackInfo = nullptr;
      const clap_host_state *_hostState = nullptr;
      const clap_host_note_name *_hostNoteName = nullptr;
      const clap_host_remote_controls *_hostRemoteControls = nullptr;
      const clap_host_voice_info *_hostVoiceInfo = nullptr;
      const clap_host_tail *_hostTail = nullptr;
      const clap_host_context_menu *_hostContextMenu = nullptr;
      const clap_host_preset_load *_hostPresetLoad = nullptr;
   };
}} // namespace clap::helpers
