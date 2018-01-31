find_path(GRPC_INCLUDE_DIR grpc/grpc.h)
find_library(GPR_LIBRARY gpr)
find_library(GRPC_LIBRARY grpc)
find_library(GRPCXX_LIBRARY grpc++)

# plugins
find_program(GRPC_CXX_PLUGIN grpc_cpp_plugin)
find_program(GRPC_PYTHON_PLUGIN grpc_python_plugin)

if(GRPC_INCLUDE_DIR AND
   GPR_LIBRARY AND
   GRPC_LIBRARY AND
   GRPCXX_LIBRARY AND
   GRPC_CXX_PLUGIN AND
   GRPC_PYTHON_PLUGIN)
    set(Grpc_INCLUDE_DIRS ${GRPC_INCLUDE_DIR})
    set(Grpc_LIBRARIES ${GPR_LIBRARY} ${GRPC_LIBRARY} ${GRPCXX_LIBRARY})
    set(Grpc_CXX_PLUGIN ${GRPC_CXX_PLUGIN})
    set(Grpc_PYTHON_PLUGIN ${GRPC_PYTHON_PLUGIN})
endif()

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Grpc
    REQUIRED_VARS
        Grpc_INCLUDE_DIRS
        Grpc_LIBRARIES
        Grpc_CXX_PLUGIN
        Grpc_PYTHON_PLUGIN
)
