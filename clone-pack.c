begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"pkt-line.h"
end_include
begin_include
include|#
directive|include
file|<sys/wait.h>
end_include
begin_decl_stmt
DECL|variable|quiet
specifier|static
name|int
name|quiet
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|keep_pack
specifier|static
name|int
name|keep_pack
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|clone_pack_usage
specifier|static
specifier|const
name|char
name|clone_pack_usage
index|[]
init|=
literal|"git-clone-pack [-q] [--keep] [--exec=<git-upload-pack>] [<host>:]<directory> [<heads>]*"
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|exec
specifier|static
specifier|const
name|char
modifier|*
name|exec
init|=
literal|"git-upload-pack"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|clone_handshake
specifier|static
name|void
name|clone_handshake
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
parameter_list|,
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
while|while
condition|(
name|ref
condition|)
block|{
name|packet_write
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|,
literal|"want %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|ref
operator|=
name|ref
operator|->
name|next
expr_stmt|;
block|}
name|packet_flush
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
comment|/* We don't have nuttin' */
name|packet_write
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|,
literal|"done\n"
argument_list|)
expr_stmt|;
if|if
condition|(
name|get_ack
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
name|sha1
argument_list|)
condition|)
name|error
argument_list|(
literal|"Huh! git-clone-pack got positive ack for %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|is_master
specifier|static
name|int
name|is_master
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
block|{
return|return
operator|!
name|strcmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"refs/heads/master"
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|write_one_ref
specifier|static
name|void
name|write_one_ref
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
block|{
name|char
modifier|*
name|path
init|=
name|git_path
argument_list|(
literal|"%s"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|char
modifier|*
name|hex
decl_stmt|;
if|if
condition|(
operator|!
name|strncmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"refs/"
argument_list|,
literal|5
argument_list|)
operator|&&
name|check_ref_format
argument_list|(
name|ref
operator|->
name|name
operator|+
literal|5
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"refusing to create funny ref '%s' locally"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|safe_create_leading_directories
argument_list|(
name|path
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to create leading directory for %s"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|fd
operator|=
name|open
argument_list|(
name|path
argument_list|,
name|O_CREAT
operator||
name|O_EXCL
operator||
name|O_WRONLY
argument_list|,
literal|0666
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to create ref %s"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|hex
operator|=
name|sha1_to_hex
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
name|hex
index|[
literal|40
index|]
operator|=
literal|'\n'
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
name|hex
argument_list|,
literal|41
argument_list|)
operator|!=
literal|41
condition|)
name|die
argument_list|(
literal|"unable to write ref %s"
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|write_refs
specifier|static
name|void
name|write_refs
parameter_list|(
name|struct
name|ref
modifier|*
name|ref
parameter_list|)
block|{
name|struct
name|ref
modifier|*
name|head
init|=
name|NULL
decl_stmt|,
modifier|*
name|head_ptr
decl_stmt|,
modifier|*
name|master_ref
decl_stmt|;
name|char
modifier|*
name|head_path
decl_stmt|;
comment|/* Upload-pack must report HEAD first */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"HEAD"
argument_list|)
condition|)
block|{
name|head
operator|=
name|ref
expr_stmt|;
name|ref
operator|=
name|ref
operator|->
name|next
expr_stmt|;
block|}
name|head_ptr
operator|=
name|NULL
expr_stmt|;
name|master_ref
operator|=
name|NULL
expr_stmt|;
while|while
condition|(
name|ref
condition|)
block|{
if|if
condition|(
name|is_master
argument_list|(
name|ref
argument_list|)
condition|)
name|master_ref
operator|=
name|ref
expr_stmt|;
if|if
condition|(
name|head
operator|&&
operator|!
name|memcmp
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|,
name|head
operator|->
name|old_sha1
argument_list|,
literal|20
argument_list|)
operator|&&
operator|!
name|strncmp
argument_list|(
name|ref
operator|->
name|name
argument_list|,
literal|"refs/heads/"
argument_list|,
literal|11
argument_list|)
operator|&&
operator|(
operator|!
name|head_ptr
operator|||
name|ref
operator|==
name|master_ref
operator|)
condition|)
name|head_ptr
operator|=
name|ref
expr_stmt|;
name|write_one_ref
argument_list|(
name|ref
argument_list|)
expr_stmt|;
name|ref
operator|=
name|ref
operator|->
name|next
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|head
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"No HEAD in remote.\n"
argument_list|)
expr_stmt|;
return|return;
block|}
name|head_path
operator|=
name|strdup
argument_list|(
name|git_path
argument_list|(
literal|"HEAD"
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|head_ptr
condition|)
block|{
comment|/* 		 * If we had a master ref, and it wasn't HEAD, we need to undo the 		 * symlink, and write a standalone HEAD. Give a warning, because that's 		 * really really wrong. 		 */
if|if
condition|(
name|master_ref
condition|)
block|{
name|error
argument_list|(
literal|"HEAD doesn't point to any refs! Making standalone HEAD"
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|head_path
argument_list|)
expr_stmt|;
block|}
name|write_one_ref
argument_list|(
name|head
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|head_path
argument_list|)
expr_stmt|;
return|return;
block|}
comment|/* We reset to the master branch if it's available */
if|if
condition|(
name|master_ref
condition|)
return|return;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Setting HEAD to %s\n"
argument_list|,
name|head_ptr
operator|->
name|name
argument_list|)
expr_stmt|;
comment|/* 	 * Uhhuh. Other end didn't have master. We start HEAD off with 	 * the first branch with the same value. 	 */
if|if
condition|(
name|create_symref
argument_list|(
name|head_path
argument_list|,
name|head_ptr
operator|->
name|name
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"unable to link HEAD to %s"
argument_list|,
name|head_ptr
operator|->
name|name
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|head_path
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|clone_by_unpack
specifier|static
name|int
name|clone_by_unpack
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
parameter_list|)
block|{
name|int
name|status
decl_stmt|;
name|pid_t
name|pid
decl_stmt|;
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
literal|"git-clone-pack: unable to fork off git-unpack-objects"
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
name|execlp
argument_list|(
literal|"git-unpack-objects"
argument_list|,
literal|"git-unpack-objects"
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
literal|"Sherlock Holmes! git-unpack-objects died of unnatural causes %d!"
argument_list|,
name|status
argument_list|)
expr_stmt|;
block|}
end_function
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
literal|"git-clone-pack: unable to set up pipe"
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
literal|"git-clone-pack: unable to fork off git-index-pack"
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
name|execlp
argument_list|(
literal|"git-index-pack"
argument_list|,
literal|"git-index-pack"
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
literal|"git-clone-pack: unable to read from git-index-pack"
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
name|int
name|retval
init|=
name|waitpid
argument_list|(
name|pid
argument_list|,
operator|&
name|status
argument_list|,
literal|0
argument_list|)
decl_stmt|;
if|if
condition|(
name|retval
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
name|retval
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
DECL|function|clone_without_unpack
specifier|static
name|int
name|clone_without_unpack
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
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
decl_stmt|;
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
literal|"%s/pack-XXXXXX"
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
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|clone_pack
specifier|static
name|int
name|clone_pack
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
parameter_list|,
name|int
name|nr_match
parameter_list|,
name|char
modifier|*
modifier|*
name|match
parameter_list|)
block|{
name|struct
name|ref
modifier|*
name|refs
decl_stmt|;
name|int
name|status
decl_stmt|;
name|get_remote_heads
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
operator|&
name|refs
argument_list|,
name|nr_match
argument_list|,
name|match
argument_list|,
literal|1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|refs
condition|)
block|{
name|packet_flush
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|die
argument_list|(
literal|"no matching remote head"
argument_list|)
expr_stmt|;
block|}
name|clone_handshake
argument_list|(
name|fd
argument_list|,
name|refs
argument_list|)
expr_stmt|;
if|if
condition|(
name|keep_pack
condition|)
name|status
operator|=
name|clone_without_unpack
argument_list|(
name|fd
argument_list|)
expr_stmt|;
else|else
name|status
operator|=
name|clone_by_unpack
argument_list|(
name|fd
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|status
condition|)
name|write_refs
argument_list|(
name|refs
argument_list|)
expr_stmt|;
return|return
name|status
return|;
block|}
end_function
begin_function
DECL|function|clone_options
specifier|static
name|int
name|clone_options
parameter_list|(
specifier|const
name|char
modifier|*
name|var
parameter_list|,
specifier|const
name|char
modifier|*
name|value
parameter_list|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"clone.keeppack"
argument_list|,
name|var
argument_list|)
condition|)
block|{
name|keep_pack
operator|=
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"clone.quiet"
argument_list|,
name|var
argument_list|)
condition|)
block|{
name|quiet
operator|=
name|git_config_bool
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
comment|/* 	 * Put other local option parsing for this program 	 * here ... 	 */
comment|/* Fall back on the default ones */
return|return
name|git_default_config
argument_list|(
name|var
argument_list|,
name|value
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|main
name|int
name|main
parameter_list|(
name|int
name|argc
parameter_list|,
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|ret
decl_stmt|,
name|nr_heads
decl_stmt|;
name|char
modifier|*
name|dest
init|=
name|NULL
decl_stmt|,
modifier|*
modifier|*
name|heads
decl_stmt|;
name|int
name|fd
index|[
literal|2
index|]
decl_stmt|;
name|pid_t
name|pid
decl_stmt|;
name|git_config
argument_list|(
name|clone_options
argument_list|)
expr_stmt|;
name|nr_heads
operator|=
literal|0
expr_stmt|;
name|heads
operator|=
name|NULL
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<
name|argc
condition|;
name|i
operator|++
control|)
block|{
name|char
modifier|*
name|arg
init|=
name|argv
index|[
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|*
name|arg
operator|==
literal|'-'
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"-q"
argument_list|,
name|arg
argument_list|)
condition|)
block|{
name|quiet
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strncmp
argument_list|(
literal|"--exec="
argument_list|,
name|arg
argument_list|,
literal|7
argument_list|)
condition|)
block|{
name|exec
operator|=
name|arg
operator|+
literal|7
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|strcmp
argument_list|(
literal|"--keep"
argument_list|,
name|arg
argument_list|)
condition|)
block|{
name|keep_pack
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|clone_pack_usage
argument_list|)
expr_stmt|;
block|}
name|dest
operator|=
name|arg
expr_stmt|;
name|heads
operator|=
name|argv
operator|+
name|i
operator|+
literal|1
expr_stmt|;
name|nr_heads
operator|=
name|argc
operator|-
name|i
operator|-
literal|1
expr_stmt|;
break|break;
block|}
if|if
condition|(
operator|!
name|dest
condition|)
name|usage
argument_list|(
name|clone_pack_usage
argument_list|)
expr_stmt|;
name|pid
operator|=
name|git_connect
argument_list|(
name|fd
argument_list|,
name|dest
argument_list|,
name|exec
argument_list|)
expr_stmt|;
if|if
condition|(
name|pid
operator|<
literal|0
condition|)
return|return
literal|1
return|;
name|ret
operator|=
name|clone_pack
argument_list|(
name|fd
argument_list|,
name|nr_heads
argument_list|,
name|heads
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
name|finish_connect
argument_list|(
name|pid
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
end_unit
