include(FetchContent)

FetchContent_Declare(
  user_mode
  GIT_REPOSITORY https://github.com/sirofen/aion_user.git
  GIT_TAG        master
  HTTP_HEADER
)

FetchContent_MakeAvailable(user_mode)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE user_mode)
