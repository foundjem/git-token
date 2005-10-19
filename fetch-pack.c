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
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|<time.h>
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
DECL|variable|verbose
specifier|static
name|int
name|verbose
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|fetch_pack_usage
specifier|static
specifier|const
name|char
name|fetch_pack_usage
index|[]
init|=
literal|"git-fetch-pack [-q] [-v] [--exec=upload-pack] [host:]directory<refs>..."
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
DECL|function|find_common
specifier|static
name|int
name|find_common
parameter_list|(
name|int
name|fd
index|[
literal|2
index|]
parameter_list|,
name|unsigned
name|char
modifier|*
name|result_sha1
parameter_list|,
name|struct
name|ref
modifier|*
name|refs
parameter_list|)
block|{
name|int
name|fetching
decl_stmt|;
specifier|static
name|char
name|line
index|[
literal|1000
index|]
decl_stmt|;
name|int
name|count
init|=
literal|0
decl_stmt|,
name|flushes
init|=
literal|0
decl_stmt|,
name|retval
decl_stmt|;
name|FILE
modifier|*
name|revs
decl_stmt|;
name|revs
operator|=
name|popen
argument_list|(
literal|"git-rev-list $(git-rev-parse --all)"
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|revs
condition|)
name|die
argument_list|(
literal|"unable to run 'git-rev-list'"
argument_list|)
expr_stmt|;
name|fetching
operator|=
literal|0
expr_stmt|;
for|for
control|(
init|;
name|refs
condition|;
name|refs
operator|=
name|refs
operator|->
name|next
control|)
block|{
name|unsigned
name|char
modifier|*
name|remote
init|=
name|refs
operator|->
name|old_sha1
decl_stmt|;
name|unsigned
name|char
modifier|*
name|local
init|=
name|refs
operator|->
name|new_sha1
decl_stmt|;
if|if
condition|(
operator|!
name|memcmp
argument_list|(
name|remote
argument_list|,
name|local
argument_list|,
literal|20
argument_list|)
condition|)
continue|continue;
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
name|remote
argument_list|)
argument_list|)
expr_stmt|;
name|fetching
operator|++
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
if|if
condition|(
operator|!
name|fetching
condition|)
return|return
literal|1
return|;
name|flushes
operator|=
literal|1
expr_stmt|;
name|retval
operator|=
operator|-
literal|1
expr_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|line
argument_list|,
sizeof|sizeof
argument_list|(
name|line
argument_list|)
argument_list|,
name|revs
argument_list|)
operator|!=
name|NULL
condition|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|line
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"git-fetch-pack: expected object name, got crud"
argument_list|)
expr_stmt|;
name|packet_write
argument_list|(
name|fd
index|[
literal|1
index|]
argument_list|,
literal|"have %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"have %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
literal|31
operator|&
operator|++
name|count
operator|)
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
name|flushes
operator|++
expr_stmt|;
comment|/* 			 * We keep one window "ahead" of the other side, and 			 * will wait for an ACK only on the next one 			 */
if|if
condition|(
name|count
operator|==
literal|32
condition|)
continue|continue;
if|if
condition|(
name|get_ack
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
name|result_sha1
argument_list|)
condition|)
block|{
name|flushes
operator|=
literal|0
expr_stmt|;
name|retval
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"got ack\n"
argument_list|)
expr_stmt|;
break|break;
block|}
name|flushes
operator|--
expr_stmt|;
block|}
block|}
name|pclose
argument_list|(
name|revs
argument_list|)
expr_stmt|;
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
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"done\n"
argument_list|)
expr_stmt|;
while|while
condition|(
name|flushes
condition|)
block|{
name|flushes
operator|--
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
name|result_sha1
argument_list|)
condition|)
block|{
if|if
condition|(
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"got ack\n"
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
block|}
return|return
name|retval
return|;
block|}
end_function
begin_define
DECL|macro|COMPLETE
define|#
directive|define
name|COMPLETE
value|(1U<< 0)
end_define
begin_decl_stmt
DECL|variable|complete
specifier|static
name|struct
name|commit_list
modifier|*
name|complete
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|mark_complete
specifier|static
name|int
name|mark_complete
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object
modifier|*
name|o
init|=
name|parse_object
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
while|while
condition|(
name|o
operator|&&
name|o
operator|->
name|type
operator|==
name|tag_type
condition|)
block|{
name|o
operator|->
name|flags
operator||=
name|COMPLETE
expr_stmt|;
name|o
operator|=
name|parse_object
argument_list|(
operator|(
operator|(
expr|struct
name|tag
operator|*
operator|)
name|o
operator|)
operator|->
name|tagged
operator|->
name|sha1
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|o
operator|->
name|type
operator|==
name|commit_type
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|o
decl_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|COMPLETE
expr_stmt|;
name|insert_by_date
argument_list|(
name|commit
argument_list|,
operator|&
name|complete
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|mark_recent_complete_commits
specifier|static
name|void
name|mark_recent_complete_commits
parameter_list|(
name|unsigned
name|long
name|cutoff
parameter_list|)
block|{
while|while
condition|(
name|complete
operator|&&
name|cutoff
operator|<=
name|complete
operator|->
name|item
operator|->
name|date
condition|)
block|{
if|if
condition|(
name|verbose
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Marking %s as complete\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|complete
operator|->
name|item
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|pop_most_recent_commit
argument_list|(
operator|&
name|complete
argument_list|,
name|COMPLETE
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|everything_local
specifier|static
name|int
name|everything_local
parameter_list|(
name|struct
name|ref
modifier|*
name|refs
parameter_list|)
block|{
name|struct
name|ref
modifier|*
name|ref
decl_stmt|;
name|int
name|retval
decl_stmt|;
name|unsigned
name|long
name|cutoff
init|=
literal|0
decl_stmt|;
name|track_object_refs
operator|=
literal|0
expr_stmt|;
name|save_commit_buffer
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|ref
operator|=
name|refs
init|;
name|ref
condition|;
name|ref
operator|=
name|ref
operator|->
name|next
control|)
block|{
name|struct
name|object
modifier|*
name|o
decl_stmt|;
name|o
operator|=
name|parse_object
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|o
condition|)
continue|continue;
comment|/* We already have it -- which may mean that we were 		 * in sync with the other side at some time after 		 * that (it is OK if we guess wrong here). 		 */
if|if
condition|(
name|o
operator|->
name|type
operator|==
name|commit_type
condition|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|o
decl_stmt|;
if|if
condition|(
operator|!
name|cutoff
operator|||
name|cutoff
operator|<
name|commit
operator|->
name|date
condition|)
name|cutoff
operator|=
name|commit
operator|->
name|date
expr_stmt|;
block|}
block|}
name|for_each_ref
argument_list|(
name|mark_complete
argument_list|)
expr_stmt|;
if|if
condition|(
name|cutoff
condition|)
name|mark_recent_complete_commits
argument_list|(
name|cutoff
argument_list|)
expr_stmt|;
for|for
control|(
name|retval
operator|=
literal|1
init|;
name|refs
condition|;
name|refs
operator|=
name|refs
operator|->
name|next
control|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|remote
init|=
name|refs
operator|->
name|old_sha1
decl_stmt|;
name|unsigned
name|char
name|local
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|object
modifier|*
name|o
decl_stmt|;
name|o
operator|=
name|parse_object
argument_list|(
name|remote
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|o
operator|||
operator|!
operator|(
name|o
operator|->
name|flags
operator|&
name|COMPLETE
operator|)
condition|)
block|{
name|retval
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|verbose
condition|)
continue|continue;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"want %s (%s)\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|remote
argument_list|)
argument_list|,
name|refs
operator|->
name|name
argument_list|)
expr_stmt|;
continue|continue;
block|}
name|memcpy
argument_list|(
name|refs
operator|->
name|new_sha1
argument_list|,
name|local
argument_list|,
literal|20
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|verbose
condition|)
continue|continue;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"already have %s (%s)\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|remote
argument_list|)
argument_list|,
name|refs
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
return|return
name|retval
return|;
block|}
end_function
begin_function
DECL|function|fetch_pack
specifier|static
name|int
name|fetch_pack
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
name|ref
decl_stmt|;
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|status
decl_stmt|;
name|pid_t
name|pid
decl_stmt|;
name|get_remote_heads
argument_list|(
name|fd
index|[
literal|0
index|]
argument_list|,
operator|&
name|ref
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
name|ref
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
if|if
condition|(
name|everything_local
argument_list|(
name|ref
argument_list|)
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
goto|goto
name|all_done
goto|;
block|}
if|if
condition|(
name|find_common
argument_list|(
name|fd
argument_list|,
name|sha1
argument_list|,
name|ref
argument_list|)
operator|<
literal|0
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"warning: no common commits\n"
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
literal|"git-fetch-pack: unable to fork off git-unpack-objects"
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
name|all_done
label|:
while|while
condition|(
name|ref
condition|)
block|{
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|ref
operator|->
name|old_sha1
argument_list|)
argument_list|,
name|ref
operator|->
name|name
argument_list|)
expr_stmt|;
name|ref
operator|=
name|ref
operator|->
name|next
expr_stmt|;
block|}
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
name|strcmp
argument_list|(
literal|"-v"
argument_list|,
name|arg
argument_list|)
condition|)
block|{
name|verbose
operator|=
literal|1
expr_stmt|;
continue|continue;
block|}
name|usage
argument_list|(
name|fetch_pack_usage
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
name|fetch_pack_usage
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
name|fetch_pack
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
