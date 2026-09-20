idf_component_register(SRCS "thread.c" "classtable.c" "converter.c" "memman.c" "interpreter.c" "class.c" "monitor.c" "heap.c" "native_methods_service.c" "loader.c" "init.c" "stringpool.c" "bumper.c"
                            "parser.c" "main.c" "stream.c" "natives/java/lang/Object.c" "natives/java/lang/System.c" "natives/java/lang/String.c" "natives/java/io/NativeOutputStream.c" "jstringpool.c"
                           
                    INCLUDE_DIRS ".")
target_compile_options(${COMPONENT_LIB} PRIVATE -Wno-unused-value)