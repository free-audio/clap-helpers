#pragma once

#include <clap/ext/draft/context-menu.h>

namespace clap { namespace helpers {

   class ContextMenuBuilder {
   public:
      ContextMenuBuilder() = default;

      // not copyable, not moveable
      ContextMenuBuilder(const ContextMenuBuilder &) = delete;
      ContextMenuBuilder(ContextMenuBuilder &&) = delete;
      ContextMenuBuilder &operator=(const ContextMenuBuilder &) = delete;
      ContextMenuBuilder &operator=(ContextMenuBuilder &&) = delete;

      virtual bool addItem(clap_context_menu_item_kind_t item_kind, const void *item_data) {
         return false;
      }

      virtual bool supports(clap_context_menu_item_kind_t item_kind) { return false; }

      const clap_context_menu_builder *builder() const noexcept { return &_builder; }

   private:
      static bool clapAddItem(const struct clap_context_menu_builder *builder,
                              clap_context_menu_item_kind_t item_kind,
                              const void *item_data) {
         auto *self = static_cast<ContextMenuBuilder *>(builder->ctx);
         return self->addItem(item_kind, item_data);
      }

      static bool clapSupports(const struct clap_context_menu_builder *builder,
                               clap_context_menu_item_kind_t item_kind) {
         auto *self = static_cast<ContextMenuBuilder *>(builder->ctx);
         return self->supports(item_kind);
      }

      const clap_context_menu_builder _builder = {this, &clapAddItem, &clapSupports};
   };

}} // namespace clap::helpers
