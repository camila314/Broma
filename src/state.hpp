#pragma once

#include <tao/pegtl.hpp>
#include <ast.hpp>

namespace broma {
	template <typename Rule>
	struct run_action {};

	struct ScratchData {
		bool is_class;
		Class wip_class;
		FunctionProto wip_fn_proto;
		Field wip_field;
		PlatformNumber wip_bind;
		Platform wip_bind_platform;
		Type wip_type;
		MemberFunctionProto wip_mem_fn_proto;

		std::vector<tao::pegtl::parse_error> errors;
	};
} // namespace broma
