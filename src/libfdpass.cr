@[Link(ldflags: "#{__DIR__}/fdpass.o")]
lib LibFDPass
  fun send_fd(unix_fd : LibC::Int, fd : LibC::Int) : LibC::Int
  fun send_fds(unix_fd : LibC::Int, fds : Void*, fds_len : LibC::Int, text : LibC::Char*) : LibC::Int
  fun recv_fd(unix_fd : LibC::Int) : LibC::Int
  fun recv_fds(unix_fd : LibC::Int, fds : Void*, len : LibC::Int*) : LibC::Int
end
