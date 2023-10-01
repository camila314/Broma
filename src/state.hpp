#pragma once

#include <tao/pegtl.hpp>
#include <ast.hpp>

namespace broma {
	/// @brief Base class for all Broma actions.
	///
	/// Template specializations of this class are used as "callbacks"
	/// so that the AST can be changed when the parser reaches an appropriate stage.
	/// See "How Broma uses PEGTL" in the Developer's Guide for more information.
	template <typename Rule>
	struct run_action {};

	/// @brief Temporary parsed data to be manipulated in `run_action` specializations.
	///
	/// If the parser successfully parses a grammar, data here will be written into the final AST.
	struct ScratchData {
		bool is_class;
		Class wip_class;
		FunctionProto wip_fn_proto;
		Field wip_field;
		PlatformNumber wip_bind;
		Platform wip_bind_platform;
		Type wip_type;
		MemberFunctionProto wip_mem_fn_proto;
		Platform wip_link_platform = Platform::None;
		Platform wip_missing_platform = Platform::None;

		std::vector<tao::pegtl::parse_error> errors;
	};
} // namespace broma
