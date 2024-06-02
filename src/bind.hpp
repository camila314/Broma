#pragma once

#include <tao/pegtl.hpp>
using namespace tao::pegtl;

#include "basic_components.hpp"
#include "state.hpp"

namespace broma {
	/// @brief A bound offset expression.
	struct bind :
		seq<rule_begin<bind>, opt_must<
			one<'='>,
			sep, 
			list<opt<
				sep,
				tagged_platform<bind>,
				sep,
				tagged_rule<bind, hex>
			>, one<','>>,
			sep
		>, one<';'>> {};

	template <>
	struct run_action<rule_begin<bind>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			// Res
			scratch->wip_bind = PlatformNumber();
		}
	};

	template <>
	struct run_action<tagged_platform<bind>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_bind_platform = str_to_platform(input.string());
		}
	};

	template <>
	struct run_action<tagged_rule<bind, hex>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			size_t out = std::stoul(input.string(), nullptr, 16);

			switch (scratch->wip_bind_platform) {
				// define mac as intel mac to avoid exploding the older bindings
				case Platform::Mac:
					scratch->wip_bind.imac = out;
					break;
				case Platform::MacIntel:
					scratch->wip_bind.imac = out;
					break;
				case Platform::MacArm:
					scratch->wip_bind.m1 = out;
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
} // namespace broma
