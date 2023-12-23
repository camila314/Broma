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

	struct docs_attribute : seq<ascii::string<'/', '*'>, tagged_rule<docs_attribute, string_literal>, until<seq<ascii::string<'*', '/'>>>> /*basic_attribute<TAO_PEGTL_KEYWORD("docs"), tagged_rule<docs_attribute, string_literal>>*/ {};
	struct depends_attribute : basic_attribute<TAO_PEGTL_KEYWORD("depends"), tagged_rule<depends_attribute, qualified>> {};

	template <typename Attribute>
	struct platform_list : seq<
		sep, 
		list<opt<
			sep,
			tagged_platform<Attribute>,
			sep
		>, one<','>>,
		sep
	> {};

	struct link_attribute : basic_attribute<TAO_PEGTL_KEYWORD("link"), tagged_rule<link_attribute, 
		seq<
			rule_begin<link_attribute>,
			opt<platform_list<link_attribute>>
		>
	>> {};

	struct missing_attribute : basic_attribute<TAO_PEGTL_KEYWORD("missing"), tagged_rule<missing_attribute,
		seq<
			rule_begin<missing_attribute>,
			opt<platform_list<missing_attribute>>
		>
	>> {};

	/// @brief All allowed C++ attributes.
	///
	/// Currently, this includes the `docs(...)`, `depends(...)`, `link(...)` and `missing(...)` attributes.
	struct attribute : 
		seq<
			ascii::string<'[', '['>,
			if_then_else<
				at<ascii::string<']', ']'>>,
				success,
				list<seq<
					sep,
					sor<docs_attribute, depends_attribute, link_attribute, missing_attribute>,
					sep
				>, one<','>>
			>,
			ascii::string<']', ']'>
		> {};

	// depends

	template <>
	struct run_action<tagged_rule<depends_attribute, qualified>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_class.depends.push_back(input.string());
		}
	};

	// docs

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

	// link

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
	template <>
	struct run_action<tagged_rule<link_attribute, keyword_android64>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_link_platform |= Platform::Android64;
		}
	};

	template <>
	struct run_action<rule_begin<link_attribute>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_link_platform = Platform::None;
		}
	};

	// missing

	template <>
	struct run_action<tagged_rule<missing_attribute, keyword_mac>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_missing_platform |= Platform::Mac;
		}
	};

	template <>
	struct run_action<tagged_rule<missing_attribute, keyword_ios>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_missing_platform |= Platform::iOS;
		}
	};

	template <>
	struct run_action<tagged_rule<missing_attribute, keyword_win>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_missing_platform |= Platform::Windows;
		}
	};

	template <>
	struct run_action<tagged_rule<missing_attribute, keyword_android>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_missing_platform |= Platform::Android;
		}
	};
	template <>
	struct run_action<tagged_rule<missing_attribute, keyword_android64>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_missing_platform |= Platform::Android64;
		}
	};

	template <>
	struct run_action<rule_begin<missing_attribute>> {
		template <typename T>
		static void apply(T& input, Root* root, ScratchData* scratch) {
			scratch->wip_missing_platform = Platform::None;
		}
	};
} // namespace broma
