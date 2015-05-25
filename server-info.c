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
file|"object.h"
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
begin_comment
comment|/*  * Create the file "path" by writing to a temporary file and renaming  * it into place. The contents of the file come from "generate", which  * should return non-zero if it encounters an error.  */
end_comment
begin_function
DECL|function|update_info_file
specifier|static
name|int
name|update_info_file
parameter_list|(
name|char
modifier|*
name|path
parameter_list|,
name|int
function_decl|(
modifier|*
name|generate
function_decl|)
parameter_list|(
name|FILE
modifier|*
parameter_list|)
parameter_list|)
block|{
name|char
modifier|*
name|tmp
init|=
name|mkpathdup
argument_list|(
literal|"%s_XXXXXX"
argument_list|,
name|path
argument_list|)
decl_stmt|;
name|int
name|ret
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|fd
init|=
operator|-
literal|1
decl_stmt|;
name|FILE
modifier|*
name|fp
init|=
name|NULL
decl_stmt|;
name|safe_create_leading_directories
argument_list|(
name|path
argument_list|)
expr_stmt|;
name|fd
operator|=
name|git_mkstemp_mode
argument_list|(
name|tmp
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
goto|goto
name|out
goto|;
name|fp
operator|=
name|fdopen
argument_list|(
name|fd
argument_list|,
literal|"w"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|fp
condition|)
goto|goto
name|out
goto|;
name|ret
operator|=
name|generate
argument_list|(
name|fp
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
condition|)
goto|goto
name|out
goto|;
if|if
condition|(
name|fclose
argument_list|(
name|fp
argument_list|)
condition|)
goto|goto
name|out
goto|;
if|if
condition|(
name|adjust_shared_perm
argument_list|(
name|tmp
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|out
goto|;
if|if
condition|(
name|rename
argument_list|(
name|tmp
argument_list|,
name|path
argument_list|)
operator|<
literal|0
condition|)
goto|goto
name|out
goto|;
name|ret
operator|=
literal|0
expr_stmt|;
name|out
label|:
if|if
condition|(
name|ret
condition|)
block|{
name|error
argument_list|(
literal|"unable to update %s: %s"
argument_list|,
name|path
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|fp
condition|)
name|fclose
argument_list|(
name|fp
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|fd
operator|>=
literal|0
condition|)
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|unlink
argument_list|(
name|tmp
argument_list|)
expr_stmt|;
block|}
name|free
argument_list|(
name|tmp
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|add_info_ref
specifier|static
name|int
name|add_info_ref
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
specifier|const
name|struct
name|object_id
modifier|*
name|oid
parameter_list|,
name|int
name|flag
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|FILE
modifier|*
name|fp
init|=
name|cb_data
decl_stmt|;
name|struct
name|object
modifier|*
name|o
init|=
name|parse_object
argument_list|(
name|oid
operator|->
name|hash
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|o
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|fprintf
argument_list|(
name|fp
argument_list|,
literal|"%s	%s\n"
argument_list|,
name|oid_to_hex
argument_list|(
name|oid
argument_list|)
argument_list|,
name|path
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|o
operator|->
name|type
operator|==
name|OBJ_TAG
condition|)
block|{
name|o
operator|=
name|deref_tag
argument_list|(
name|o
argument_list|,
name|path
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|o
condition|)
if|if
condition|(
name|fprintf
argument_list|(
name|fp
argument_list|,
literal|"%s	%s^{}\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|o
operator|->
name|sha1
argument_list|)
argument_list|,
name|path
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|generate_info_refs
specifier|static
name|int
name|generate_info_refs
parameter_list|(
name|FILE
modifier|*
name|fp
parameter_list|)
block|{
return|return
name|for_each_ref
argument_list|(
name|add_info_ref
argument_list|,
name|fp
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|update_info_refs
specifier|static
name|int
name|update_info_refs
parameter_list|(
name|int
name|force
parameter_list|)
block|{
name|char
modifier|*
name|path
init|=
name|git_pathdup
argument_list|(
literal|"info/refs"
argument_list|)
decl_stmt|;
name|int
name|ret
init|=
name|update_info_file
argument_list|(
name|path
argument_list|,
name|generate_info_refs
argument_list|)
decl_stmt|;
name|free
argument_list|(
name|path
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_comment
comment|/* packs */
end_comment
begin_struct
DECL|struct|pack_info
specifier|static
struct|struct
name|pack_info
block|{
DECL|member|p
name|struct
name|packed_git
modifier|*
name|p
decl_stmt|;
DECL|member|old_num
name|int
name|old_num
decl_stmt|;
DECL|member|new_num
name|int
name|new_num
decl_stmt|;
DECL|member|nr_alloc
name|int
name|nr_alloc
decl_stmt|;
DECL|member|nr_heads
name|int
name|nr_heads
decl_stmt|;
DECL|member|head
name|unsigned
name|char
argument_list|(
operator|*
name|head
argument_list|)
decl|[20]
decl_stmt|;
block|}
DECL|variable|info
modifier|*
modifier|*
name|info
struct|;
end_struct
begin_decl_stmt
DECL|variable|num_pack
specifier|static
name|int
name|num_pack
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|objdir
specifier|static
specifier|const
name|char
modifier|*
name|objdir
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|objdirlen
specifier|static
name|int
name|objdirlen
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|find_pack_by_name
specifier|static
name|struct
name|pack_info
modifier|*
name|find_pack_by_name
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_pack
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|packed_git
modifier|*
name|p
init|=
name|info
index|[
name|i
index|]
operator|->
name|p
decl_stmt|;
comment|/* skip "/pack/" after ".git/objects" */
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|p
operator|->
name|pack_name
operator|+
name|objdirlen
operator|+
literal|6
argument_list|,
name|name
argument_list|)
condition|)
return|return
name|info
index|[
name|i
index|]
return|;
block|}
return|return
name|NULL
return|;
block|}
end_function
begin_comment
comment|/* Returns non-zero when we detect that the info in the  * old file is useless.  */
end_comment
begin_function
DECL|function|parse_pack_def
specifier|static
name|int
name|parse_pack_def
parameter_list|(
specifier|const
name|char
modifier|*
name|line
parameter_list|,
name|int
name|old_cnt
parameter_list|)
block|{
name|struct
name|pack_info
modifier|*
name|i
init|=
name|find_pack_by_name
argument_list|(
name|line
operator|+
literal|2
argument_list|)
decl_stmt|;
if|if
condition|(
name|i
condition|)
block|{
name|i
operator|->
name|old_num
operator|=
name|old_cnt
expr_stmt|;
return|return
literal|0
return|;
block|}
else|else
block|{
comment|/* The file describes a pack that is no longer here */
return|return
literal|1
return|;
block|}
block|}
end_function
begin_comment
comment|/* Returns non-zero when we detect that the info in the  * old file is useless.  */
end_comment
begin_function
DECL|function|read_pack_info_file
specifier|static
name|int
name|read_pack_info_file
parameter_list|(
specifier|const
name|char
modifier|*
name|infofile
parameter_list|)
block|{
name|FILE
modifier|*
name|fp
decl_stmt|;
name|char
name|line
index|[
literal|1000
index|]
decl_stmt|;
name|int
name|old_cnt
init|=
literal|0
decl_stmt|;
name|fp
operator|=
name|fopen
argument_list|(
name|infofile
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|fp
condition|)
return|return
literal|1
return|;
comment|/* nonexistent is not an error. */
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
name|fp
argument_list|)
condition|)
block|{
name|int
name|len
init|=
name|strlen
argument_list|(
name|line
argument_list|)
decl_stmt|;
if|if
condition|(
name|len
operator|&&
name|line
index|[
name|len
operator|-
literal|1
index|]
operator|==
literal|'\n'
condition|)
name|line
index|[
operator|--
name|len
index|]
operator|=
literal|0
expr_stmt|;
if|if
condition|(
operator|!
name|len
condition|)
continue|continue;
switch|switch
condition|(
name|line
index|[
literal|0
index|]
condition|)
block|{
case|case
literal|'P'
case|:
comment|/* P name */
if|if
condition|(
name|parse_pack_def
argument_list|(
name|line
argument_list|,
name|old_cnt
operator|++
argument_list|)
condition|)
goto|goto
name|out_stale
goto|;
break|break;
case|case
literal|'D'
case|:
comment|/* we used to emit D but that was misguided. */
case|case
literal|'T'
case|:
comment|/* we used to emit T but nobody uses it. */
goto|goto
name|out_stale
goto|;
default|default:
name|error
argument_list|(
literal|"unrecognized: %s"
argument_list|,
name|line
argument_list|)
expr_stmt|;
break|break;
block|}
block|}
name|fclose
argument_list|(
name|fp
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
name|out_stale
label|:
name|fclose
argument_list|(
name|fp
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|compare_info
specifier|static
name|int
name|compare_info
parameter_list|(
specifier|const
name|void
modifier|*
name|a_
parameter_list|,
specifier|const
name|void
modifier|*
name|b_
parameter_list|)
block|{
name|struct
name|pack_info
modifier|*
specifier|const
modifier|*
name|a
init|=
name|a_
decl_stmt|;
name|struct
name|pack_info
modifier|*
specifier|const
modifier|*
name|b
init|=
name|b_
decl_stmt|;
if|if
condition|(
literal|0
operator|<=
operator|(
operator|*
name|a
operator|)
operator|->
name|old_num
operator|&&
literal|0
operator|<=
operator|(
operator|*
name|b
operator|)
operator|->
name|old_num
condition|)
comment|/* Keep the order in the original */
return|return
operator|(
operator|*
name|a
operator|)
operator|->
name|old_num
operator|-
operator|(
operator|*
name|b
operator|)
operator|->
name|old_num
return|;
elseif|else
if|if
condition|(
literal|0
operator|<=
operator|(
operator|*
name|a
operator|)
operator|->
name|old_num
condition|)
comment|/* Only A existed in the original so B is obviously newer */
return|return
operator|-
literal|1
return|;
elseif|else
if|if
condition|(
literal|0
operator|<=
operator|(
operator|*
name|b
operator|)
operator|->
name|old_num
condition|)
comment|/* The other way around. */
return|return
literal|1
return|;
comment|/* then it does not matter but at least keep the comparison stable */
if|if
condition|(
operator|(
operator|*
name|a
operator|)
operator|->
name|p
operator|==
operator|(
operator|*
name|b
operator|)
operator|->
name|p
condition|)
return|return
literal|0
return|;
elseif|else
if|if
condition|(
operator|(
operator|*
name|a
operator|)
operator|->
name|p
operator|<
operator|(
operator|*
name|b
operator|)
operator|->
name|p
condition|)
return|return
operator|-
literal|1
return|;
else|else
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|init_pack_info
specifier|static
name|void
name|init_pack_info
parameter_list|(
specifier|const
name|char
modifier|*
name|infofile
parameter_list|,
name|int
name|force
parameter_list|)
block|{
name|struct
name|packed_git
modifier|*
name|p
decl_stmt|;
name|int
name|stale
decl_stmt|;
name|int
name|i
init|=
literal|0
decl_stmt|;
name|objdir
operator|=
name|get_object_directory
argument_list|()
expr_stmt|;
name|objdirlen
operator|=
name|strlen
argument_list|(
name|objdir
argument_list|)
expr_stmt|;
name|prepare_packed_git
argument_list|()
expr_stmt|;
for|for
control|(
name|p
operator|=
name|packed_git
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
comment|/* we ignore things on alternate path since they are 		 * not available to the pullers in general. 		 */
if|if
condition|(
operator|!
name|p
operator|->
name|pack_local
condition|)
continue|continue;
name|i
operator|++
expr_stmt|;
block|}
name|num_pack
operator|=
name|i
expr_stmt|;
name|info
operator|=
name|xcalloc
argument_list|(
name|num_pack
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|pack_info
operator|*
argument_list|)
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
operator|,
name|p
operator|=
name|packed_git
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
name|pack_local
condition|)
continue|continue;
name|info
index|[
name|i
index|]
operator|=
name|xcalloc
argument_list|(
literal|1
argument_list|,
sizeof|sizeof
argument_list|(
expr|struct
name|pack_info
argument_list|)
argument_list|)
expr_stmt|;
name|info
index|[
name|i
index|]
operator|->
name|p
operator|=
name|p
expr_stmt|;
name|info
index|[
name|i
index|]
operator|->
name|old_num
operator|=
operator|-
literal|1
expr_stmt|;
name|i
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|infofile
operator|&&
operator|!
name|force
condition|)
name|stale
operator|=
name|read_pack_info_file
argument_list|(
name|infofile
argument_list|)
expr_stmt|;
else|else
name|stale
operator|=
literal|1
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_pack
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|stale
condition|)
block|{
name|info
index|[
name|i
index|]
operator|->
name|old_num
operator|=
operator|-
literal|1
expr_stmt|;
name|info
index|[
name|i
index|]
operator|->
name|nr_heads
operator|=
literal|0
expr_stmt|;
block|}
block|}
comment|/* renumber them */
name|qsort
argument_list|(
name|info
argument_list|,
name|num_pack
argument_list|,
sizeof|sizeof
argument_list|(
name|info
index|[
literal|0
index|]
argument_list|)
argument_list|,
name|compare_info
argument_list|)
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_pack
condition|;
name|i
operator|++
control|)
name|info
index|[
name|i
index|]
operator|->
name|new_num
operator|=
name|i
expr_stmt|;
block|}
end_function
begin_function
DECL|function|free_pack_info
specifier|static
name|void
name|free_pack_info
parameter_list|(
name|void
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_pack
condition|;
name|i
operator|++
control|)
name|free
argument_list|(
name|info
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|info
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|write_pack_info_file
specifier|static
name|int
name|write_pack_info_file
parameter_list|(
name|FILE
modifier|*
name|fp
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|num_pack
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|fprintf
argument_list|(
name|fp
argument_list|,
literal|"P %s\n"
argument_list|,
name|info
index|[
name|i
index|]
operator|->
name|p
operator|->
name|pack_name
operator|+
name|objdirlen
operator|+
literal|6
argument_list|)
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|fputc
argument_list|(
literal|'\n'
argument_list|,
name|fp
argument_list|)
operator|==
name|EOF
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|update_info_packs
specifier|static
name|int
name|update_info_packs
parameter_list|(
name|int
name|force
parameter_list|)
block|{
name|char
modifier|*
name|infofile
init|=
name|mkpathdup
argument_list|(
literal|"%s/info/packs"
argument_list|,
name|get_object_directory
argument_list|()
argument_list|)
decl_stmt|;
name|int
name|ret
decl_stmt|;
name|init_pack_info
argument_list|(
name|infofile
argument_list|,
name|force
argument_list|)
expr_stmt|;
name|ret
operator|=
name|update_info_file
argument_list|(
name|infofile
argument_list|,
name|write_pack_info_file
argument_list|)
expr_stmt|;
name|free_pack_info
argument_list|()
expr_stmt|;
name|free
argument_list|(
name|infofile
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_comment
comment|/* public */
end_comment
begin_function
DECL|function|update_server_info
name|int
name|update_server_info
parameter_list|(
name|int
name|force
parameter_list|)
block|{
comment|/* We would add more dumb-server support files later, 	 * including index of available pack files and their 	 * intended audiences. 	 */
name|int
name|errs
init|=
literal|0
decl_stmt|;
name|errs
operator|=
name|errs
operator||
name|update_info_refs
argument_list|(
name|force
argument_list|)
expr_stmt|;
name|errs
operator|=
name|errs
operator||
name|update_info_packs
argument_list|(
name|force
argument_list|)
expr_stmt|;
comment|/* remove leftover rev-cache file if there is any */
name|unlink_or_warn
argument_list|(
name|git_path
argument_list|(
literal|"info/rev-cache"
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|errs
return|;
block|}
end_function
end_unit
