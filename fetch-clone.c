begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_include
include|#
directive|include
file|"sideband.h"
end_include
begin_include
include|#
directive|include
file|<sys/wait.h>
end_include
begin_include
include|#
directive|include
file|<sys/time.h>
end_include
begin_function
DECL|function|finish_pack
specifier|static
name|int
name|finish_pack
parameter_list|(
specifier|const
name|char
modifier|*
name|pack_tmp_name
parameter_list|,
specifier|const
name|char
modifier|*
name|me
parameter_list|)
block|{
name|int
name|pipe_fd
index|[
literal|2
index|]
decl_stmt|;
name|pid_t
name|pid
decl_stmt|;
name|char
name|idx
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|char
name|final
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|char
name|hash
index|[
literal|41
index|]
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|char
modifier|*
name|cp
decl_stmt|;
name|int
name|err
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|pipe
argument_list|(
name|pipe_fd
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"%s: unable to set up pipe"
argument_list|,
name|me
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|idx
argument_list|,
name|pack_tmp_name
argument_list|)
expr_stmt|;
comment|/* ".git/objects/pack-XXXXXX" */
name|cp
operator|=
name|strrchr
argument_list|(
name|idx
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|cp
argument_list|,
literal|"/pidx"
argument_list|,
literal|5
argument_list|)
expr_stmt|;
name|pid
operator|=
name|fork
argument_list|()
expr_stmt|;
if|if
condition|(
name|pid
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"%s: unable to fork off git-index-pack"
argument_list|,
name|me
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pid
condition|)
block|{
name|close
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|dup2
argument_list|(
name|pipe_fd
index|[
literal|1
index|]
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|pipe_fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|pipe_fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|execl_git_cmd
argument_list|(
literal|"index-pack"
argument_list|,
literal|"-o"
argument_list|,
name|idx
argument_list|,
name|pack_tmp_name
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|error
argument_list|(
literal|"cannot exec git-index-pack<%s><%s>"
argument_list|,
name|idx
argument_list|,
name|pack_tmp_name
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|pipe_fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|read
argument_list|(
name|pipe_fd
index|[
literal|0
index|]
argument_list|,
name|hash
argument_list|,
literal|40
argument_list|)
operator|!=
literal|40
condition|)
block|{
name|error
argument_list|(
literal|"%s: unable to read from git-index-pack"
argument_list|,
name|me
argument_list|)
expr_stmt|;
name|err
operator|=
literal|1
expr_stmt|;
block|}
name|close
argument_list|(
name|pipe_fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|int
name|status
decl_stmt|,
name|code
decl_stmt|;
if|if
condition|(
name|waitpid
argument_list|(
name|pid
argument_list|,
operator|&
name|status
argument_list|,
literal|0
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|EINTR
condition|)
continue|continue;
name|error
argument_list|(
literal|"waitpid failed (%s)"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
goto|goto
name|error_die
goto|;
block|}
if|if
condition|(
name|WIFSIGNALED
argument_list|(
name|status
argument_list|)
condition|)
block|{
name|int
name|sig
init|=
name|WTERMSIG
argument_list|(
name|status
argument_list|)
decl_stmt|;
name|error
argument_list|(
literal|"git-index-pack died of signal %d"
argument_list|,
name|sig
argument_list|)
expr_stmt|;
goto|goto
name|error_die
goto|;
block|}
if|if
condition|(
operator|!
name|WIFEXITED
argument_list|(
name|status
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"git-index-pack died of unnatural causes %d"
argument_list|,
name|status
argument_list|)
expr_stmt|;
goto|goto
name|error_die
goto|;
block|}
name|code
operator|=
name|WEXITSTATUS
argument_list|(
name|status
argument_list|)
expr_stmt|;
if|if
condition|(
name|code
condition|)
block|{
name|error
argument_list|(
literal|"git-index-pack died with error code %d"
argument_list|,
name|code
argument_list|)
expr_stmt|;
goto|goto
name|error_die
goto|;
block|}
if|if
condition|(
name|err
condition|)
goto|goto
name|error_die
goto|;
break|break;
block|}
name|hash
index|[
literal|40
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|hash
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"git-index-pack reported nonsense '%s'"
argument_list|,
name|hash
argument_list|)
expr_stmt|;
goto|goto
name|error_die
goto|;
block|}
comment|/* Now we have pack in pack_tmp_name[], and 	 * idx in idx[]; rename them to their final names. 	 */
name|snprintf
argument_list|(
name|final
argument_list|,
sizeof|sizeof
argument_list|(
name|final
argument_list|)
argument_list|,
literal|"%s/pack/pack-%s.pack"
argument_list|,
name|get_object_directory
argument_list|()
argument_list|,
name|hash
argument_list|)
expr_stmt|;
name|move_temp_to_file
argument_list|(
name|pack_tmp_name
argument_list|,
name|final
argument_list|)
expr_stmt|;
name|chmod
argument_list|(
name|final
argument_list|,
literal|0444
argument_list|)
expr_stmt|;
name|snprintf
argument_list|(
name|final
argument_list|,
sizeof|sizeof
argument_list|(
name|final
argument_list|)
argument_list|,
literal|"%s/pack/pack-%s.idx"
argument_list|,
name|get_object_directory
argument_list|()
argument_list|,
name|hash
argument_list|)
expr_stmt|;
name|move_temp_to_file
argument_list|(
name|idx
argument_list|,
name|final
argument_list|)
expr_stmt|;
name|chmod
argument_list|(
name|final
argument_list|,
literal|0444
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
name|error_die
label|:
name|unlink
argument_list|(
name|idx
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|pack_tmp_name
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|setup_sideband
specifier|static
name|pid_t
name|setup_sideband
parameter_list|(
name|int
name|sideband
parameter_list|,
specifier|const
name|char
modifier|*
name|me
parameter_list|,
name|int
name|fd
index|[
literal|2
index|]
parameter_list|,
name|int
name|xd
index|[
literal|2
index|]
parameter_list|)
block|{
name|pid_t
name|side_pid
decl_stmt|;
if|if
condition|(
operator|!
name|sideband
condition|)
block|{
name|fd
index|[
literal|0
index|]
operator|=
name|xd
index|[
literal|0
index|]
expr_stmt|;
name|fd
index|[
literal|1
index|]
operator|=
name|xd
index|[
literal|1
index|]
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* xd[] is talking with upload-pack; subprocess reads from 	 * xd[0], spits out band#2 to stderr, and feeds us band#1 	 * through our fd[0]. 	 */
if|if
condition|(
name|pipe
argument_list|(
name|fd
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"%s: unable to set up pipe"
argument_list|,
name|me
argument_list|)
expr_stmt|;
name|side_pid
operator|=
name|fork
argument_list|()
expr_stmt|;
if|if
condition|(
name|side_pid
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"%s: unable to fork off sideband demultiplexer"
argument_list|,
name|me
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|side_pid
condition|)
block|{
comment|/* subprocess */
name|char
name|buf
index|[
name|LARGE_PACKET_MAX
index|]
decl_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|xd
index|[
literal|0
index|]
operator|!=
name|xd
index|[
literal|1
index|]
condition|)
name|close
argument_list|(
name|xd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|recv_sideband
argument_list|(
name|me
argument_list|,
name|xd
index|[
literal|0
index|]
argument_list|,
name|fd
index|[
literal|1
index|]
argument_list|,
literal|2
argument_list|,
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|)
condition|)
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|xd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|fd
index|[
literal|1
index|]
operator|=
name|xd
index|[
literal|1
index|]
expr_stmt|;
return|return
name|side_pid
return|;
block|}
end_function
begin_function
DECL|function|receive_unpack_pack
name|int
name|receive_unpack_pack
parameter_list|(
name|int
name|xd
index|[
literal|2
index|]
parameter_list|,
specifier|const
name|char
modifier|*
name|me
parameter_list|,
name|int
name|quiet
parameter_list|,
name|int
name|sideband
parameter_list|)
block|{
name|int
name|status
decl_stmt|;
name|pid_t
name|pid
decl_stmt|,
name|side_pid
decl_stmt|;
name|int
name|fd
index|[
literal|2
index|]
decl_stmt|;
name|side_pid
operator|=
name|setup_sideband
argument_list|(
name|sideband
argument_list|,
name|me
argument_list|,
name|fd
argument_list|,
name|xd
argument_list|)
expr_stmt|;
name|pid
operator|=
name|fork
argument_list|()
expr_stmt|;
if|if
condition|(
name|pid
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"%s: unable to fork off git-unpack-objects"
argument_list|,
name|me
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pid
condition|)
block|{
name|dup2
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|execl_git_cmd
argument_list|(
literal|"unpack-objects"
argument_list|,
name|quiet
condition|?
literal|"-q"
else|:
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"git-unpack-objects exec failed"
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
while|while
condition|(
name|waitpid
argument_list|(
name|pid
argument_list|,
operator|&
name|status
argument_list|,
literal|0
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|EINTR
condition|)
name|die
argument_list|(
literal|"waiting for git-unpack-objects: %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|WIFEXITED
argument_list|(
name|status
argument_list|)
condition|)
block|{
name|int
name|code
init|=
name|WEXITSTATUS
argument_list|(
name|status
argument_list|)
decl_stmt|;
if|if
condition|(
name|code
condition|)
name|die
argument_list|(
literal|"git-unpack-objects died with error code %d"
argument_list|,
name|code
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|WIFSIGNALED
argument_list|(
name|status
argument_list|)
condition|)
block|{
name|int
name|sig
init|=
name|WTERMSIG
argument_list|(
name|status
argument_list|)
decl_stmt|;
name|die
argument_list|(
literal|"git-unpack-objects died of signal %d"
argument_list|,
name|sig
argument_list|)
expr_stmt|;
block|}
name|die
argument_list|(
literal|"git-unpack-objects died of unnatural causes %d"
argument_list|,
name|status
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * We average out the download speed over this many "events", where  * an event is a minimum of about half a second. That way, we get  * a reasonably stable number.  */
end_comment
begin_define
DECL|macro|NR_AVERAGE
define|#
directive|define
name|NR_AVERAGE
value|(4)
end_define
begin_comment
comment|/*  * A "binary msec" is a power-of-two-msec, aka 1/1024th of a second.  * Keeping the time in that format means that "bytes / msecs" means  * the same as kB/s (modulo rounding).  *  * 1000512 is a magic number (usecs in a second, rounded up by half  * of 1024, to make "rounding" come out right ;)  */
end_comment
begin_define
DECL|macro|usec_to_binarymsec
define|#
directive|define
name|usec_to_binarymsec
parameter_list|(
name|x
parameter_list|)
value|((int)(x) / (1000512>> 10))
end_define
begin_function
DECL|function|receive_keep_pack
name|int
name|receive_keep_pack
parameter_list|(
name|int
name|xd
index|[
literal|2
index|]
parameter_list|,
specifier|const
name|char
modifier|*
name|me
parameter_list|,
name|int
name|quiet
parameter_list|,
name|int
name|sideband
parameter_list|)
block|{
name|char
name|tmpfile
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|int
name|ofd
decl_stmt|,
name|ifd
decl_stmt|,
name|fd
index|[
literal|2
index|]
decl_stmt|;
name|unsigned
name|long
name|total
decl_stmt|;
specifier|static
name|struct
name|timeval
name|prev_tv
decl_stmt|;
struct|struct
name|average
block|{
name|unsigned
name|long
name|bytes
decl_stmt|;
name|unsigned
name|long
name|time
decl_stmt|;
block|}
name|download
index|[
name|NR_AVERAGE
index|]
init|=
block|{
block|{
literal|0
block|,
literal|0
block|}
block|, }
struct|;
name|unsigned
name|long
name|avg_bytes
decl_stmt|,
name|avg_time
decl_stmt|;
name|int
name|idx
init|=
literal|0
decl_stmt|;
name|setup_sideband
argument_list|(
name|sideband
argument_list|,
name|me
argument_list|,
name|fd
argument_list|,
name|xd
argument_list|)
expr_stmt|;
name|ifd
operator|=
name|fd
index|[
literal|0
index|]
expr_stmt|;
name|snprintf
argument_list|(
name|tmpfile
argument_list|,
sizeof|sizeof
argument_list|(
name|tmpfile
argument_list|)
argument_list|,
literal|"%s/pack/tmp-XXXXXX"
argument_list|,
name|get_object_directory
argument_list|()
argument_list|)
expr_stmt|;
name|ofd
operator|=
name|mkstemp
argument_list|(
name|tmpfile
argument_list|)
expr_stmt|;
if|if
condition|(
name|ofd
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"unable to create temporary file %s"
argument_list|,
name|tmpfile
argument_list|)
return|;
name|gettimeofday
argument_list|(
operator|&
name|prev_tv
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|total
operator|=
literal|0
expr_stmt|;
name|avg_bytes
operator|=
literal|0
expr_stmt|;
name|avg_time
operator|=
literal|0
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|char
name|buf
index|[
literal|8192
index|]
decl_stmt|;
name|ssize_t
name|sz
decl_stmt|,
name|wsz
decl_stmt|,
name|pos
decl_stmt|;
name|sz
operator|=
name|read
argument_list|(
name|ifd
argument_list|,
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|sz
operator|==
literal|0
condition|)
break|break;
if|if
condition|(
name|sz
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|!=
name|EINTR
operator|&&
name|errno
operator|!=
name|EAGAIN
condition|)
block|{
name|error
argument_list|(
literal|"error reading pack (%s)"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|ofd
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|tmpfile
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|sz
operator|=
literal|0
expr_stmt|;
block|}
name|pos
operator|=
literal|0
expr_stmt|;
while|while
condition|(
name|pos
operator|<
name|sz
condition|)
block|{
name|wsz
operator|=
name|write
argument_list|(
name|ofd
argument_list|,
name|buf
operator|+
name|pos
argument_list|,
name|sz
operator|-
name|pos
argument_list|)
expr_stmt|;
if|if
condition|(
name|wsz
operator|<
literal|0
condition|)
block|{
name|error
argument_list|(
literal|"error writing pack (%s)"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|ofd
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|tmpfile
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
name|pos
operator|+=
name|wsz
expr_stmt|;
block|}
name|total
operator|+=
name|sz
expr_stmt|;
if|if
condition|(
operator|!
name|quiet
condition|)
block|{
specifier|static
name|unsigned
name|long
name|last
decl_stmt|;
name|struct
name|timeval
name|tv
decl_stmt|;
name|unsigned
name|long
name|diff
init|=
name|total
operator|-
name|last
decl_stmt|;
comment|/* not really "msecs", but a power-of-two millisec (1/1024th of a sec) */
name|unsigned
name|long
name|msecs
decl_stmt|;
name|gettimeofday
argument_list|(
operator|&
name|tv
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|msecs
operator|=
name|tv
operator|.
name|tv_sec
operator|-
name|prev_tv
operator|.
name|tv_sec
expr_stmt|;
name|msecs
operator|<<=
literal|10
expr_stmt|;
name|msecs
operator|+=
name|usec_to_binarymsec
argument_list|(
name|tv
operator|.
name|tv_usec
operator|-
name|prev_tv
operator|.
name|tv_usec
argument_list|)
expr_stmt|;
if|if
condition|(
name|msecs
operator|>
literal|500
condition|)
block|{
name|prev_tv
operator|=
name|tv
expr_stmt|;
name|last
operator|=
name|total
expr_stmt|;
comment|/* Update averages ..*/
name|avg_bytes
operator|+=
name|diff
expr_stmt|;
name|avg_time
operator|+=
name|msecs
expr_stmt|;
name|avg_bytes
operator|-=
name|download
index|[
name|idx
index|]
operator|.
name|bytes
expr_stmt|;
name|avg_time
operator|-=
name|download
index|[
name|idx
index|]
operator|.
name|time
expr_stmt|;
name|download
index|[
name|idx
index|]
operator|.
name|bytes
operator|=
name|diff
expr_stmt|;
name|download
index|[
name|idx
index|]
operator|.
name|time
operator|=
name|msecs
expr_stmt|;
name|idx
operator|++
expr_stmt|;
if|if
condition|(
name|idx
operator|>=
name|NR_AVERAGE
condition|)
name|idx
operator|=
literal|0
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"%4lu.%03luMB  (%lu kB/s)      \r"
argument_list|,
name|total
operator|>>
literal|20
argument_list|,
literal|1000
operator|*
operator|(
operator|(
name|total
operator|>>
literal|10
operator|)
operator|&
literal|1023
operator|)
operator|>>
literal|10
argument_list|,
name|avg_bytes
operator|/
name|avg_time
argument_list|)
expr_stmt|;
block|}
block|}
block|}
name|close
argument_list|(
name|ofd
argument_list|)
expr_stmt|;
return|return
name|finish_pack
argument_list|(
name|tmpfile
argument_list|,
name|me
argument_list|)
return|;
block|}
end_function
end_unit
