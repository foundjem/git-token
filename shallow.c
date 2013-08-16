begin_unit
begin_include
include|#
directive|include
file|"cache.h"
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
file|"pkt-line.h"
end_include
begin_decl_stmt
DECL|variable|is_shallow
specifier|static
name|int
name|is_shallow
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|shallow_stat
specifier|static
name|struct
name|stat
name|shallow_stat
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|alternate_shallow_file
specifier|static
name|char
modifier|*
name|alternate_shallow_file
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|set_alternate_shallow_file
name|void
name|set_alternate_shallow_file
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
if|if
condition|(
name|is_shallow
operator|!=
operator|-
literal|1
condition|)
name|die
argument_list|(
literal|"BUG: is_repository_shallow must not be called before set_alternate_shallow_file"
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|alternate_shallow_file
argument_list|)
expr_stmt|;
name|alternate_shallow_file
operator|=
name|path
condition|?
name|xstrdup
argument_list|(
name|path
argument_list|)
else|:
name|NULL
expr_stmt|;
block|}
end_function
begin_function
DECL|function|register_shallow
name|int
name|register_shallow
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|commit_graft
modifier|*
name|graft
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|commit_graft
argument_list|)
argument_list|)
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
init|=
name|lookup_commit
argument_list|(
name|sha1
argument_list|)
decl_stmt|;
name|hashcpy
argument_list|(
name|graft
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|graft
operator|->
name|nr_parent
operator|=
operator|-
literal|1
expr_stmt|;
if|if
condition|(
name|commit
operator|&&
name|commit
operator|->
name|object
operator|.
name|parsed
condition|)
name|commit
operator|->
name|parents
operator|=
name|NULL
expr_stmt|;
return|return
name|register_commit_graft
argument_list|(
name|graft
argument_list|,
literal|0
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|is_repository_shallow
name|int
name|is_repository_shallow
parameter_list|(
name|void
parameter_list|)
block|{
name|FILE
modifier|*
name|fp
decl_stmt|;
name|char
name|buf
index|[
literal|1024
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|path
init|=
name|alternate_shallow_file
decl_stmt|;
if|if
condition|(
name|is_shallow
operator|>=
literal|0
condition|)
return|return
name|is_shallow
return|;
if|if
condition|(
operator|!
name|path
condition|)
name|path
operator|=
name|git_path
argument_list|(
literal|"shallow"
argument_list|)
expr_stmt|;
comment|/* 	 * fetch-pack sets '--shallow-file ""' as an indicator that no 	 * shallow file should be used. We could just open it and it 	 * will likely fail. But let's do an explicit check instead. 	 */
if|if
condition|(
operator|!
operator|*
name|path
operator|||
name|stat
argument_list|(
name|path
argument_list|,
operator|&
name|shallow_stat
argument_list|)
operator|||
operator|(
name|fp
operator|=
name|fopen
argument_list|(
name|path
argument_list|,
literal|"r"
argument_list|)
operator|)
operator|==
name|NULL
condition|)
block|{
name|is_shallow
operator|=
literal|0
expr_stmt|;
return|return
name|is_shallow
return|;
block|}
name|is_shallow
operator|=
literal|1
expr_stmt|;
while|while
condition|(
name|fgets
argument_list|(
name|buf
argument_list|,
sizeof|sizeof
argument_list|(
name|buf
argument_list|)
argument_list|,
name|fp
argument_list|)
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
name|buf
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
literal|"bad shallow line: %s"
argument_list|,
name|buf
argument_list|)
expr_stmt|;
name|register_shallow
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
block|}
name|fclose
argument_list|(
name|fp
argument_list|)
expr_stmt|;
return|return
name|is_shallow
return|;
block|}
end_function
begin_function
DECL|function|get_shallow_commits
name|struct
name|commit_list
modifier|*
name|get_shallow_commits
parameter_list|(
name|struct
name|object_array
modifier|*
name|heads
parameter_list|,
name|int
name|depth
parameter_list|,
name|int
name|shallow_flag
parameter_list|,
name|int
name|not_shallow_flag
parameter_list|)
block|{
name|int
name|i
init|=
literal|0
decl_stmt|,
name|cur_depth
init|=
literal|0
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|result
init|=
name|NULL
decl_stmt|;
name|struct
name|object_array
name|stack
init|=
name|OBJECT_ARRAY_INIT
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
init|=
name|NULL
decl_stmt|;
while|while
condition|(
name|commit
operator|||
name|i
operator|<
name|heads
operator|->
name|nr
operator|||
name|stack
operator|.
name|nr
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
if|if
condition|(
operator|!
name|commit
condition|)
block|{
if|if
condition|(
name|i
operator|<
name|heads
operator|->
name|nr
condition|)
block|{
name|commit
operator|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|deref_tag
argument_list|(
name|heads
operator|->
name|objects
index|[
name|i
operator|++
index|]
operator|.
name|item
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|commit
operator|||
name|commit
operator|->
name|object
operator|.
name|type
operator|!=
name|OBJ_COMMIT
condition|)
block|{
name|commit
operator|=
name|NULL
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|!
name|commit
operator|->
name|util
condition|)
name|commit
operator|->
name|util
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|int
argument_list|)
argument_list|)
expr_stmt|;
operator|*
operator|(
name|int
operator|*
operator|)
name|commit
operator|->
name|util
operator|=
literal|0
expr_stmt|;
name|cur_depth
operator|=
literal|0
expr_stmt|;
block|}
else|else
block|{
name|commit
operator|=
operator|(
expr|struct
name|commit
operator|*
operator|)
name|stack
operator|.
name|objects
index|[
operator|--
name|stack
operator|.
name|nr
index|]
operator|.
name|item
expr_stmt|;
name|cur_depth
operator|=
operator|*
operator|(
name|int
operator|*
operator|)
name|commit
operator|->
name|util
expr_stmt|;
block|}
block|}
if|if
condition|(
name|parse_commit
argument_list|(
name|commit
argument_list|)
condition|)
name|die
argument_list|(
literal|"invalid commit"
argument_list|)
expr_stmt|;
name|cur_depth
operator|++
expr_stmt|;
if|if
condition|(
name|cur_depth
operator|>=
name|depth
condition|)
block|{
name|commit_list_insert
argument_list|(
name|commit
argument_list|,
operator|&
name|result
argument_list|)
expr_stmt|;
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|shallow_flag
expr_stmt|;
name|commit
operator|=
name|NULL
expr_stmt|;
continue|continue;
block|}
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|not_shallow_flag
expr_stmt|;
for|for
control|(
name|p
operator|=
name|commit
operator|->
name|parents
operator|,
name|commit
operator|=
name|NULL
init|;
name|p
condition|;
name|p
operator|=
name|p
operator|->
name|next
control|)
block|{
if|if
condition|(
operator|!
name|p
operator|->
name|item
operator|->
name|util
condition|)
block|{
name|int
modifier|*
name|pointer
init|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
name|int
argument_list|)
argument_list|)
decl_stmt|;
name|p
operator|->
name|item
operator|->
name|util
operator|=
name|pointer
expr_stmt|;
operator|*
name|pointer
operator|=
name|cur_depth
expr_stmt|;
block|}
else|else
block|{
name|int
modifier|*
name|pointer
init|=
name|p
operator|->
name|item
operator|->
name|util
decl_stmt|;
if|if
condition|(
name|cur_depth
operator|>=
operator|*
name|pointer
condition|)
continue|continue;
operator|*
name|pointer
operator|=
name|cur_depth
expr_stmt|;
block|}
if|if
condition|(
name|p
operator|->
name|next
condition|)
name|add_object_array
argument_list|(
operator|&
name|p
operator|->
name|item
operator|->
name|object
argument_list|,
name|NULL
argument_list|,
operator|&
name|stack
argument_list|)
expr_stmt|;
else|else
block|{
name|commit
operator|=
name|p
operator|->
name|item
expr_stmt|;
name|cur_depth
operator|=
operator|*
operator|(
name|int
operator|*
operator|)
name|commit
operator|->
name|util
expr_stmt|;
block|}
block|}
block|}
return|return
name|result
return|;
block|}
end_function
begin_function
DECL|function|check_shallow_file_for_update
name|void
name|check_shallow_file_for_update
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
operator|!
name|is_shallow
condition|)
return|return;
elseif|else
if|if
condition|(
name|is_shallow
operator|==
operator|-
literal|1
condition|)
name|die
argument_list|(
literal|"BUG: shallow must be initialized by now"
argument_list|)
expr_stmt|;
if|if
condition|(
name|stat
argument_list|(
name|git_path
argument_list|(
literal|"shallow"
argument_list|)
argument_list|,
operator|&
name|st
argument_list|)
condition|)
name|die
argument_list|(
literal|"shallow file was removed during fetch"
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|st
operator|.
name|st_mtime
operator|!=
name|shallow_stat
operator|.
name|st_mtime
ifdef|#
directive|ifdef
name|USE_NSEC
operator|||
name|ST_MTIME_NSEC
argument_list|(
name|st
argument_list|)
operator|!=
name|ST_MTIME_NSEC
argument_list|(
name|shallow_stat
argument_list|)
endif|#
directive|endif
condition|)
name|die
argument_list|(
literal|"shallow file was changed during fetch"
argument_list|)
expr_stmt|;
block|}
end_function
begin_struct
DECL|struct|write_shallow_data
struct|struct
name|write_shallow_data
block|{
DECL|member|out
name|struct
name|strbuf
modifier|*
name|out
decl_stmt|;
DECL|member|use_pack_protocol
name|int
name|use_pack_protocol
decl_stmt|;
DECL|member|count
name|int
name|count
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|write_one_shallow
specifier|static
name|int
name|write_one_shallow
parameter_list|(
specifier|const
name|struct
name|commit_graft
modifier|*
name|graft
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|struct
name|write_shallow_data
modifier|*
name|data
init|=
name|cb_data
decl_stmt|;
specifier|const
name|char
modifier|*
name|hex
init|=
name|sha1_to_hex
argument_list|(
name|graft
operator|->
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|graft
operator|->
name|nr_parent
operator|!=
operator|-
literal|1
condition|)
return|return
literal|0
return|;
name|data
operator|->
name|count
operator|++
expr_stmt|;
if|if
condition|(
name|data
operator|->
name|use_pack_protocol
condition|)
name|packet_buf_write
argument_list|(
name|data
operator|->
name|out
argument_list|,
literal|"shallow %s"
argument_list|,
name|hex
argument_list|)
expr_stmt|;
else|else
block|{
name|strbuf_addstr
argument_list|(
name|data
operator|->
name|out
argument_list|,
name|hex
argument_list|)
expr_stmt|;
name|strbuf_addch
argument_list|(
name|data
operator|->
name|out
argument_list|,
literal|'\n'
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|write_shallow_commits
name|int
name|write_shallow_commits
parameter_list|(
name|struct
name|strbuf
modifier|*
name|out
parameter_list|,
name|int
name|use_pack_protocol
parameter_list|)
block|{
name|struct
name|write_shallow_data
name|data
decl_stmt|;
name|data
operator|.
name|out
operator|=
name|out
expr_stmt|;
name|data
operator|.
name|use_pack_protocol
operator|=
name|use_pack_protocol
expr_stmt|;
name|data
operator|.
name|count
operator|=
literal|0
expr_stmt|;
name|for_each_commit_graft
argument_list|(
name|write_one_shallow
argument_list|,
operator|&
name|data
argument_list|)
expr_stmt|;
return|return
name|data
operator|.
name|count
return|;
block|}
end_function
begin_function
DECL|function|setup_temporary_shallow
name|char
modifier|*
name|setup_temporary_shallow
parameter_list|(
name|void
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|fd
decl_stmt|;
if|if
condition|(
name|write_shallow_commits
argument_list|(
operator|&
name|sb
argument_list|,
literal|0
argument_list|)
condition|)
block|{
name|struct
name|strbuf
name|path
init|=
name|STRBUF_INIT
decl_stmt|;
name|strbuf_addstr
argument_list|(
operator|&
name|path
argument_list|,
name|git_path
argument_list|(
literal|"shallow_XXXXXX"
argument_list|)
argument_list|)
expr_stmt|;
name|fd
operator|=
name|xmkstemp
argument_list|(
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|sb
operator|.
name|buf
argument_list|,
name|sb
operator|.
name|len
argument_list|)
operator|!=
name|sb
operator|.
name|len
condition|)
name|die_errno
argument_list|(
literal|"failed to write to %s"
argument_list|,
name|path
operator|.
name|buf
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
return|return
name|strbuf_detach
argument_list|(
operator|&
name|path
argument_list|,
name|NULL
argument_list|)
return|;
block|}
comment|/* 	 * is_repository_shallow() sees empty string as "no shallow 	 * file". 	 */
return|return
name|xstrdup
argument_list|(
literal|""
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|setup_alternate_shallow
name|void
name|setup_alternate_shallow
parameter_list|(
name|struct
name|lock_file
modifier|*
name|shallow_lock
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|alternate_shallow_file
parameter_list|)
block|{
name|struct
name|strbuf
name|sb
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|check_shallow_file_for_update
argument_list|()
expr_stmt|;
name|fd
operator|=
name|hold_lock_file_for_update
argument_list|(
name|shallow_lock
argument_list|,
name|git_path
argument_list|(
literal|"shallow"
argument_list|)
argument_list|,
name|LOCK_DIE_ON_ERROR
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_shallow_commits
argument_list|(
operator|&
name|sb
argument_list|,
literal|0
argument_list|)
condition|)
block|{
if|if
condition|(
name|write_in_full
argument_list|(
name|fd
argument_list|,
name|sb
operator|.
name|buf
argument_list|,
name|sb
operator|.
name|len
argument_list|)
operator|!=
name|sb
operator|.
name|len
condition|)
name|die_errno
argument_list|(
literal|"failed to write to %s"
argument_list|,
name|shallow_lock
operator|->
name|filename
argument_list|)
expr_stmt|;
operator|*
name|alternate_shallow_file
operator|=
name|shallow_lock
operator|->
name|filename
expr_stmt|;
block|}
else|else
comment|/* 		 * is_repository_shallow() sees empty string as "no 		 * shallow file". 		 */
operator|*
name|alternate_shallow_file
operator|=
literal|""
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|sb
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
