require "./libfdpass"

class FDPass
  def self.send_fd(unix_fd : Int32, fd : Int32)
    ret = LibFDPass.send_fd(unix_fd, fd)
    raise Socket::Error.from_errno("sendmsg") if ret < 0
    ret
  end

  def self.send_fd(socket : Socket, fd : Int32)
    raise ArgumentError.new("UNIX socket required") unless socket.family.unix?

    FDPass.send_fd(socket.fd, fd)
  end

  def self.send_fds(socket : Socket, fds : Array(Int32), message = " ")
    raise ArgumentError.new("UNIX socket required") unless socket.family.unix?

    send_fds(socket.fd, fds, message)
  end

  def self.send_fds(unix_fd : Int32, fds : Array(Int32), message = " ")
    ret = LibFDPass.send_fds(unix_fd, fds.to_unsafe, fds.size, message.check_no_null_byte)
    raise Socket::Error.from_errno("sendmsg") if ret < 0
    ret
  end

  def self.recv_fd(unix_fd : Int32) : Int32
    recv_fds(unix_fd, 1).first
  end

  def self.recv_fd(socket : Socket) : Int32
    raise ArgumentError.new("UNIX socket required") unless socket.family.unix?

    recv_fds(socket.fd, 1).first
  end

  def self.recv_fds(socket : Socket, max_fds = 1024) : Array(Int32)
    raise ArgumentError.new("UNIX socket required") unless socket.family.unix?

    recv_fds(socket.fd, max_fds)
  end

  def self.recv_fds(unix_fd : Int32, max_fds = 1024) : Array(Int32)
    len = 0
    buf = Slice(Int32).new(max_fds)
    ret = LibFDPass.recv_fds(unix_fd, buf.to_unsafe, pointerof(len))
    raise Socket::Error.from_errno("recvmsg") if ret < 0
    Array(Int32).new(len) { |i| buf[i] }
  end
end
