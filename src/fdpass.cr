@[Link(ldflags: "#{__DIR__}/fdpass.o")]
lib FDPass
  fun send_fd(unix_fd : LibC::Int, fd : LibC::Int) : LibC::Int
end
