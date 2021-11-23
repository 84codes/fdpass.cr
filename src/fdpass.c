#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <stdlib.h>

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

int send_fds(int unix_sock, int *fds, int fds_len, char *text)
{
    struct iovec iov = {
        .iov_base = text,
        .iov_len = strlen(text)
    };

    size_t cmsg_size = CMSG_SPACE(sizeof(int) * fds_len);
    char *cmsg_buf = malloc(cmsg_size);
    if (cmsg_buf == NULL) return -1;
    memset(cmsg_buf, 0, cmsg_size);

    struct msghdr msg = { 0 };
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
    msg.msg_control = cmsg_buf;
    msg.msg_controllen = cmsg_size;

    struct cmsghdr *cmsg = CMSG_FIRSTHDR(&msg);
    cmsg->cmsg_level = SOL_SOCKET;
    cmsg->cmsg_type = SCM_RIGHTS;
    cmsg->cmsg_len = CMSG_LEN(sizeof(int) * fds_len);
    memcpy(CMSG_DATA(cmsg), fds, sizeof(int) * fds_len);

    int res = sendmsg(unix_sock, &msg, 0);
    free(cmsg_buf);
    return res;
}

int recv_fds(int sockfd, int *fds, int *len) {
    struct msghdr msgh;
    struct iovec iov;
    int data;
    ssize_t nr;

    /* Allocate a char buffer for the ancillary data. See the comments
       in sendfd() */
    union {
        char   buf[CMSG_SPACE(sizeof(int))];
        struct cmsghdr align;
    } controlMsg;

    /* The 'msg_name' field can be used to obtain the address of the
       sending socket. However, we do not need this information. */
    msgh.msg_name = NULL;
    msgh.msg_namelen = 0;

    /* Specify buffer for receiving real data */
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    iov.iov_base = &data;       /* Real data is an 'int' */
    iov.iov_len = sizeof(int);

    /* Set 'msghdr' fields that describe ancillary data */
    msgh.msg_control = controlMsg.buf;
    msgh.msg_controllen = sizeof(controlMsg.buf);

    /* Receive real plus ancillary data; real data is ignored */
    nr = recvmsg(sockfd, &msgh, 0);
    if (nr == -1) return -1;

    for (struct cmsghdr *cmsgp = CMSG_FIRSTHDR(&msgh);
            cmsgp != NULL;
            cmsgp = CMSG_NXTHDR(&msgh, cmsgp)) {
        /* Check that 'cmsg_level' is as expected */
        if (cmsgp->cmsg_level != SOL_SOCKET)
            return -1;

        switch (cmsgp->cmsg_type) {
            case SCM_RIGHTS:        /* Header containing file descriptors */
                ;
                /* The number of file descriptors is the size of the control
                   message block minus the size that would be allocated for
                   a zero-length data block (i.e., the size of the 'cmsghdr'
                   structure plus padding), divided by the size of a file
                   descriptor */
                int fdCnt = (cmsgp->cmsg_len - CMSG_LEN(0)) / sizeof(int);
                memcpy(len, &fdCnt, sizeof(int));

                /* Allocate an array to hold the received file descriptors,
                   and copy file descriptors from cmsg into array */
                size_t fdAllocSize = sizeof(int) * fdCnt;
                memcpy(fds, CMSG_DATA(cmsgp), fdAllocSize);
                break;
            default:
                return -1;
        }
    }
    return nr;
}
