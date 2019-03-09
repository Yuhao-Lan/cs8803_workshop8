#!/bin/bash
mkdir rpc_generated
protoc hello.proto --cpp_out=rpc_generated
protoc hello.proto --grpc_out=rpc_generated --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin