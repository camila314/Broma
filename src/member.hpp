// Member variables and padding.
#pragma once

#include <tao/pegtl.hpp>
using namespace tao::pegtl;

#include "type.hpp"
#include "bind.hpp"
#include "state.hpp"

namespace broma {
	/// @brief A class member variable.
	struct member_expr : 
		seq<
			rule_begin<member_expr>,
			opt<tagged_platform<member_expr>>,
			type,
			whitespace,
			tagged_rule<member_expr, identifier>,
			sep,
			opt<array>,
			sep,
			one<';'>
		> {};

	template <>
	struct run_action<rule_begin<member_expr>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			MemberField f;
			scratch->wip_field.inner = f;
		}
	};

	template <>
	struct run_action<tagged_platform<member_expr>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			if (scratch->wip_platform_block.has_value()) {
				throw parse_error("cannot use this inside a platform expression", input);
				return;
			}

			scratch->wip_field.get_as<MemberField>()->platform = str_to_platform(input.string());
		}
	};

	template <>
	struct run_action<tagged_rule<member_expr, identifier>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_field.get_as<MemberField>()->name = input.string();
		}
	};

	template <>
	struct run_action<member_expr> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			if (scratch->wip_platform_block.has_value())
				scratch->wip_field.get_as<MemberField>()->platform = scratch->wip_platform_block.value();

			scratch->wip_field.get_as<MemberField>()->type = scratch->wip_type;
		}
	};

	/// @brief Padding in a class declaration.
	struct pad_expr : seq<keyword_PAD, sep, sor<bind, seq<
		one<'='>,
		sep,
		tagged_rule<pad_expr, hex>,
		one<';'>
	>>> {};

	template <>
	struct run_action<keyword_PAD> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			PadField f;
			PlatformNumber p;
			scratch->wip_field.inner = f;
			scratch->wip_bind = p;
		}
	};

	template <>
	struct run_action<tagged_rule<pad_expr, hex>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			if (!scratch->wip_platform_block.has_value()) {
				throw parse_error("must specify padding if not using platform expression", input);
				return;
			}

			size_t out = std::stoul(input.string(), nullptr, 16);

			switch (scratch->wip_platform_block.value()) {
				case Platform::Mac:
					scratch->wip_bind.mac = out;
					break;
				case Platform::iOS:
					scratch->wip_bind.ios = out;
					break;
				case Platform::Windows:
					scratch->wip_bind.win = out;
					break;
				case Platform::Android32:
					scratch->wip_bind.android32 = out;
					break;
				case Platform::Android64:
					scratch->wip_bind.android64 = out;
					break;
				default:
					break;
			}
		}
	};

	template <>
	struct run_action<pad_expr> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_field.get_as<PadField>()->amount = scratch->wip_bind;
		}
	};
} // namespace broma
