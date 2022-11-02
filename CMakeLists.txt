#CMakeLists.txt
PROJECT(http_wss)
# 查找当前目录下的所有源文件
# 并将名称保存到 DIR_SRCS 变量
aux_source_directory(. DIR_SRCS)
# 指定生成目标
add_executable(test ${DIR_SRCS})
#添加库
# link_libraries("/usr/local/lib/libcurlpp.so" "/usr/local/curl/lib/libcurl.so")
target_link_libraries(test curlpp curl)
#target_link_libraries(http_wss /usr/local/curl/lib/libcurl.so)

#cd /home/yaozai/wss/echo_client/TestHttpWss/build
#cd 
