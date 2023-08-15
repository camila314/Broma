#pragma once

#include <tao/pegtl.hpp>
using namespace tao::pegtl;

#include "basic_components.hpp"
#include "state.hpp"

namespace broma {
	/// @brief The inner portion of an attribute.
	///
	/// This includes anything within the double brackets (`[[...]]`).
	template <typename Name, typename ...Parse>
	struct basic_attribute : seq<
			Name,
			one<'('>,
			if_then_else<
				at<one<')'>>,
				success,
				seq<Parse...>
			>,
			one<')'>
		> {};

	struct docs_attribute : basic_attribute<TAO_PEGTL_KEYWORD("docs"), tagged_rule<docs_attribute, string_literal>> {};
	struct depends_attribute : basic_attribute<TAO_PEGTL_KEYWORD("depends"), tagged_rule<depends_attribute, qualified>> {};
	struct link_attribute : basic_attribute<TAO_PEGTL_KEYWORD("link"), tagged_rule<link_attribute, 
		seq<
			sep, 
			list<opt<
				sep,
				tagged_platform<link_attribute>,
				sep
			>, one<','>>,
			sep
		>
	>> {};

	/// @brief All allowed C++ attributes.
	///
	/// Currently, this includes the `docs(...)`, `depends(...)` and `link(...)` attributes.
	struct attribute : 
		seq<
			ascii::string<'[', '['>,
			if_then_else<
				at<ascii::string<']', ']'>>,
				success,
				list<seq<
					sep,
					sor<docs_attribute, depends_attribute, link_attribute>,
					sep
				>, one<','>>
			>,
			ascii::string<']', ']'>
		> {};

	template <>
	struct run_action<tagged_rule<depends_attribute, qualified>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_class.depends.push_back(input.string());
		}
	};

	template <>
	struct run_action<tagged_rule<docs_attribute, string_literal>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			auto docs = input.string().substr(1, input.string().size() - 1);
			if (scratch->is_class) {
				scratch->wip_mem_fn_proto.docs = docs;
			}
			else {
				scratch->wip_fn_proto.docs = docs;
			}
		}
	};

	template <>
	struct run_action<tagged_rule<link_attribute, keyword_mac>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_link_platform |= Platform::Mac;
		}
	};
	template <>
	struct run_action<tagged_rule<link_attribute, keyword_ios>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_link_platform |= Platform::iOS;
		}
	};
	template <>
	struct run_action<tagged_rule<link_attribute, keyword_win>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_link_platform |= Platform::Windows;
		}
	};
	template <>
	struct run_action<tagged_rule<link_attribute, keyword_android>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_link_platform |= Platform::Android;
		}
	};
} // namespace broma
