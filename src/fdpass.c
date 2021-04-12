#include <string.h>
#include <sys/socket.h>

int send_fd(int unix_sock, int fd)
{
    struct iovec iov = {.iov_base = ":)", // Must send at least one byte
                        .iov_len = 2};

    union {
        char buf[CMSG_SPACE(sizeof(fd))];
        struct cmsghdr align;
    } u;

    struct msghdr msg = {.msg_iov = &iov,
                         .msg_iovlen = 1,
                         .msg_control = u.buf,
                         .msg_controllen = sizeof(u.buf)};

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    *cmsg = (struct cmsghdr){.cmsg_level = SOL_SOCKET,
                             .cmsg_type = SCM_RIGHTS,
                             .cmsg_len = CMSG_LEN(sizeof(fd))};

    memcpy(CMSG_DATA(cmsg), &fd, sizeof(fd));

    return sendmsg(unix_sock, &msg, 0);
}
