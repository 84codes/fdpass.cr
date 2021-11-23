require "./spec_helper"
require "socket"

describe "FDPass" do
  it "can pass a single FD" do
    left, right = UNIXSocket.pair(Socket::Type::DGRAM)
    f = File.tempfile
    at_exit { f.delete }
    FDPass.send_fd(left.fd, f.fd)
    fd = FDPass.recv_fd(right.fd)
    io = IO::FileDescriptor.new(fd: fd)
    io.info.should eq f.info
  end

  it "can pass a single FD in an array" do
    left, right = UNIXSocket.pair(Socket::Type::DGRAM)
    f = File.tempfile
    at_exit { f.delete }
    fds = [f.fd]
    str = "foobar"
    res = FDPass.send_fds(left.fd, fds, str)
    p Errno.value if res < 0
    fd = FDPass.recv_fd(right.fd)
    io = IO::FileDescriptor.new(fd: fd)
    io.info.should eq f.info
  end

  it "can pass multiple FDs" do
    left, right = UNIXSocket.pair(Socket::Type::DGRAM)
    f1 = File.tempfile
    at_exit { f1.delete }
    f2 = File.tempfile
    at_exit { f2.delete }
    fds = [f1.fd, f2.fd]
    str = "foobar"
    FDPass.send_fds(left.fd, fds, str)

    recv_fds = FDPass.recv_fds(right.fd)

    recv_fds.size.should eq fds.size

    io1 = IO::FileDescriptor.new(fd: recv_fds[0])
    io1.info.should eq f1.info

    io2 = IO::FileDescriptor.new(fd: recv_fds[1])
    io2.info.should eq f2.info
  end
end
