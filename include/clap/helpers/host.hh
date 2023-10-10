#pragma once

#include <clap/clap.h>

#include "checking-level.hh"
#include "misbehaviour-handler.hh"

namespace clap { namespace helpers {
   template <MisbehaviourHandler h, CheckingLevel l>
   class Host {
   public:
      // not copyable, not moveable
      Host(const Host &) = delete;
      Host(Host &&) = delete;
      Host &operator=(const Host &) = delete;
      Host &operator=(Host &&) = delete;

      const clap_host *clapHost() const { return &_host; }

   protected:
      Host(const char *name, const char *vendor, const char *url, const char *version);
      virtual ~Host() = default;

      /////////////////////////
      // Methods to override //
      /////////////////////////

      // clap_host
      virtual void requestRestart() noexcept {}
      virtual void requestProcess() noexcept {}
      virtual void requestCallback() noexcept {}

      // clap_host_audio_ports
      bool audioPortsIsRescanFlagSupported(uint32_t flag) noexcept { return false; }
      void audioPortsRescan(uint32_t flags) noexcept {}

      // clap_host_gui
      virtual bool implementsGui() const noexcept { return false; }
      virtual void guiResizeHintsChanged() noexcept {}
      virtual bool guiRequestResize(uint32_t width, uint32_t height) noexcept { return false; }
      virtual bool guiRequestShow() noexcept { return false; }
      virtual bool guiRequestHide() noexcept { return false; }
      virtual void guiClosed(bool wasDestroyed) noexcept {}

      // clap_host_log
      virtual bool implementsLog() const noexcept { return false; }
      virtual void logLog(clap_log_severity severity, const char *message) const noexcept {}

      // clap_host_params
      virtual bool implementsParams() const noexcept { return false; }
      virtual void paramsRescan(clap_param_rescan_flags flags) noexcept {}
      virtual void paramsClear(clap_id paramId, clap_param_clear_flags flags) noexcept {}
      virtual void paramsRequestFlush() noexcept {}

      // clap_host_thread_check
      virtual bool implementsThreadCheck() const noexcept { return false; }
      virtual bool threadCheckIsMainThread() noexcept { return false; }
      virtual bool threadCheckIsAudioThread() noexcept { return false; }

      ///////////////
      // Utilities //
      ///////////////
      static Host &from(const clap_host *host) noexcept;

   private:
      const clap_host _host;

      /////////////////////
      // CLAP Interfaces //
      /////////////////////

      // clap_host
      static const void *clapGetExtension(const clap_host_t *host,
                                          const char *extension_id) noexcept;
      static void clapRequestRestart(const clap_host_t *host) noexcept;
      static void clapRequestProcess(const clap_host_t *host) noexcept;
      static void clapRequestCallback(const clap_host_t *host) noexcept;

      // clap_host_audio_ports
      bool clapAudioPortsIsRescanFlagSupported(const clap_host_t *host, uint32_t flag) noexcept;
      void clapAudioPortsRescan(const clap_host_t *host, uint32_t flags) noexcept;

      // clap_host_gui
      static void clapGuiResizeHintsChanged(const clap_host_t *host) noexcept;
      static bool clapGuiRequestResize(const clap_host_t *host,
                                       uint32_t width,
                                       uint32_t height) noexcept;
      static bool clapGuiRequestShow(const clap_host_t *host) noexcept;
      static bool clapGuiRequestHide(const clap_host_t *host) noexcept;
      static void clapGuiClosed(const clap_host_t *host, bool was_destroyed) noexcept;

      // clap_host_log
      static void clapLogLog(const clap_host_t *host,
                             clap_log_severity severity,
                             const char *message) noexcept;

      // clap_host_params
      static void clapParamsRescan(const clap_host_t *host, clap_param_rescan_flags flags) noexcept;
      static void clapParamsClear(const clap_host_t *host,
                                  clap_id param_id,
                                  clap_param_clear_flags flags) noexcept;
      static void clapParamsRequestFlush(const clap_host_t *host) noexcept;

      // clap_host_thread_check
      static bool clapThreadCheckIsMainThread(const clap_host_t *host) noexcept;
      static bool clapThreadCheckIsAudioThread(const clap_host_t *host) noexcept;

      // interfaces
      static const clap_host_audio_ports _hostAudioPorts;
      static const clap_host_gui _hostGui;
      static const clap_host_log _hostLog;
      static const clap_host_params _hostParams;
      static const clap_host_thread_check _hostThreadCheck;
   };
}} // namespace clap::helpers
