add_library(PROTOBUF STATIC nanopb/pb_common.c nanopb/pb_decode.c nanopb/pb_encode.c c_gen/msgs/my_msgs.pb.c )
target_include_directories(PROTOBUF PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/c_gen/msgs/ ${CMAKE_CURRENT_SOURCE_DIR}/c_gen ${CMAKE_CURRENT_SOURCE_DIR}/nanopb/)
target_link_libraries(PROTOBUF PRIVATE c)
# target_include_directories(PROTOBUF PUBLIC  nanopb/ c_gen/msgs/)

# protoc   --plugin="python3 nanopb_generator.py"   -I protobuf/   --nanopb_out=protobuf/gen/   protobuf/msgs/my_msgs.proto
# protoc      -I protobuf/   --python_out=protobuf/py_gen   protobuf/msgs/my_msgs.proto
# protoc --proto_path=src --python_out=build/gen src/foo.proto src/bar/baz.proto
