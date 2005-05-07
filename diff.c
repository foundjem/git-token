begin_unit
begin_comment
comment|/*  * Copyright (C) 2005 Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|<sys/types.h>
end_include
begin_include
include|#
directive|include
file|<sys/wait.h>
end_include
begin_include
include|#
directive|include
file|<signal.h>
end_include
begin_include
include|#
directive|include
file|<limits.h>
end_include
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_decl_stmt
DECL|variable|diff_opts
specifier|static
name|char
modifier|*
name|diff_opts
init|=
literal|"-pu"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|external_diff
specifier|static
specifier|const
name|char
modifier|*
name|external_diff
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
name|char
modifier|*
name|external_diff_cmd
init|=
name|NULL
decl_stmt|;
specifier|static
name|int
name|done_preparing
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|done_preparing
condition|)
return|return
name|external_diff_cmd
return|;
comment|/* 	 * Default values above are meant to match the 	 * Linux kernel development style.  Examples of 	 * alternative styles you can specify via environment 	 * variables are: 	 * 	 * GIT_DIFF_OPTS="-c"; 	 */
if|if
condition|(
name|getenv
argument_list|(
literal|"GIT_EXTERNAL_DIFF"
argument_list|)
condition|)
name|external_diff_cmd
operator|=
name|getenv
argument_list|(
literal|"GIT_EXTERNAL_DIFF"
argument_list|)
expr_stmt|;
comment|/* In case external diff fails... */
name|diff_opts
operator|=
name|getenv
argument_list|(
literal|"GIT_DIFF_OPTS"
argument_list|)
condition|?
else|:
name|diff_opts
expr_stmt|;
name|done_preparing
operator|=
literal|1
expr_stmt|;
return|return
name|external_diff_cmd
return|;
block|}
end_function
begin_comment
comment|/* Help to copy the thing properly quoted for the shell safety.  * any single quote is replaced with '\'', and the caller is  * expected to enclose the result within a single quote pair.  *  * E.g.  *  original     sq_expand     result  *  name     ==> name      ==> 'name'  *  a b      ==> a b       ==> 'a b'  *  a'b      ==> a'\''b    ==> 'a'\''b'  */
end_comment
begin_function
DECL|function|sq_expand
specifier|static
name|char
modifier|*
name|sq_expand
parameter_list|(
specifier|const
name|char
modifier|*
name|src
parameter_list|)
block|{
specifier|static
name|char
modifier|*
name|buf
init|=
name|NULL
decl_stmt|;
name|int
name|cnt
decl_stmt|,
name|c
decl_stmt|;
specifier|const
name|char
modifier|*
name|cp
decl_stmt|;
name|char
modifier|*
name|bp
decl_stmt|;
comment|/* count bytes needed to store the quoted string. */
for|for
control|(
name|cnt
operator|=
literal|1
operator|,
name|cp
operator|=
name|src
init|;
operator|*
name|cp
condition|;
name|cnt
operator|++
operator|,
name|cp
operator|++
control|)
if|if
condition|(
operator|*
name|cp
operator|==
literal|'\''
condition|)
name|cnt
operator|+=
literal|3
expr_stmt|;
name|buf
operator|=
name|xmalloc
argument_list|(
name|cnt
argument_list|)
expr_stmt|;
name|bp
operator|=
name|buf
expr_stmt|;
while|while
condition|(
operator|(
name|c
operator|=
operator|*
name|src
operator|++
operator|)
condition|)
block|{
if|if
condition|(
name|c
operator|!=
literal|'\''
condition|)
operator|*
name|bp
operator|++
operator|=
name|c
expr_stmt|;
else|else
block|{
name|bp
operator|=
name|strcpy
argument_list|(
name|bp
argument_list|,
literal|"'\\''"
argument_list|)
expr_stmt|;
name|bp
operator|+=
literal|4
expr_stmt|;
block|}
block|}
operator|*
name|bp
operator|=
literal|0
expr_stmt|;
return|return
name|buf
return|;
block|}
end_function
begin_struct
DECL|struct|diff_tempfile
specifier|static
struct|struct
name|diff_tempfile
block|{
DECL|member|name
specifier|const
name|char
modifier|*
name|name
decl_stmt|;
DECL|member|hex
name|char
name|hex
index|[
literal|41
index|]
decl_stmt|;
DECL|member|mode
name|char
name|mode
index|[
literal|10
index|]
decl_stmt|;
DECL|member|tmp_path
name|char
name|tmp_path
index|[
literal|50
index|]
decl_stmt|;
block|}
DECL|variable|diff_temp
name|diff_temp
index|[
literal|2
index|]
struct|;
end_struct
begin_function
DECL|function|builtin_diff
specifier|static
name|void
name|builtin_diff
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|diff_tempfile
modifier|*
name|temp
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|next_at
decl_stmt|;
specifier|const
name|char
modifier|*
name|diff_cmd
init|=
literal|"diff -L'%s%s' -L'%s%s'"
decl_stmt|;
specifier|const
name|char
modifier|*
name|diff_arg
init|=
literal|"'%s' '%s'||:"
decl_stmt|;
comment|/* "||:" is to return 0 */
specifier|const
name|char
modifier|*
name|input_name_sq
index|[
literal|2
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|path0
index|[
literal|2
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|path1
index|[
literal|2
index|]
decl_stmt|;
specifier|const
name|char
modifier|*
name|name_sq
init|=
name|sq_expand
argument_list|(
name|name
argument_list|)
decl_stmt|;
name|char
modifier|*
name|cmd
decl_stmt|;
comment|/* diff_cmd and diff_arg have 6 %s in total which makes 	 * the sum of these strings 12 bytes larger than required. 	 * we use 2 spaces around diff-opts, and we need to count 	 * terminating NUL, so we subtract 9 here. 	 */
name|int
name|cmd_size
init|=
operator|(
name|strlen
argument_list|(
name|diff_cmd
argument_list|)
operator|+
name|strlen
argument_list|(
name|diff_opts
argument_list|)
operator|+
name|strlen
argument_list|(
name|diff_arg
argument_list|)
operator|-
literal|9
operator|)
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
literal|2
condition|;
name|i
operator|++
control|)
block|{
name|input_name_sq
index|[
name|i
index|]
operator|=
name|sq_expand
argument_list|(
name|temp
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|temp
index|[
name|i
index|]
operator|.
name|name
argument_list|,
literal|"/dev/null"
argument_list|)
condition|)
block|{
name|path0
index|[
name|i
index|]
operator|=
literal|"/dev/null"
expr_stmt|;
name|path1
index|[
name|i
index|]
operator|=
literal|""
expr_stmt|;
block|}
else|else
block|{
name|path0
index|[
name|i
index|]
operator|=
name|i
condition|?
literal|"b/"
else|:
literal|"a/"
expr_stmt|;
name|path1
index|[
name|i
index|]
operator|=
name|name_sq
expr_stmt|;
block|}
name|cmd_size
operator|+=
operator|(
name|strlen
argument_list|(
name|path0
index|[
name|i
index|]
argument_list|)
operator|+
name|strlen
argument_list|(
name|path1
index|[
name|i
index|]
argument_list|)
operator|+
name|strlen
argument_list|(
name|input_name_sq
index|[
name|i
index|]
argument_list|)
operator|)
expr_stmt|;
block|}
name|cmd
operator|=
name|xmalloc
argument_list|(
name|cmd_size
argument_list|)
expr_stmt|;
name|next_at
operator|=
literal|0
expr_stmt|;
name|next_at
operator|+=
name|snprintf
argument_list|(
name|cmd
operator|+
name|next_at
argument_list|,
name|cmd_size
operator|-
name|next_at
argument_list|,
name|diff_cmd
argument_list|,
name|path0
index|[
literal|0
index|]
argument_list|,
name|path1
index|[
literal|0
index|]
argument_list|,
name|path0
index|[
literal|1
index|]
argument_list|,
name|path1
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|next_at
operator|+=
name|snprintf
argument_list|(
name|cmd
operator|+
name|next_at
argument_list|,
name|cmd_size
operator|-
name|next_at
argument_list|,
literal|" %s "
argument_list|,
name|diff_opts
argument_list|)
expr_stmt|;
name|next_at
operator|+=
name|snprintf
argument_list|(
name|cmd
operator|+
name|next_at
argument_list|,
name|cmd_size
operator|-
name|next_at
argument_list|,
name|diff_arg
argument_list|,
name|input_name_sq
index|[
literal|0
index|]
argument_list|,
name|input_name_sq
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|path1
index|[
literal|0
index|]
index|[
literal|0
index|]
condition|)
name|printf
argument_list|(
literal|"Created: %s (mode:%s)\n"
argument_list|,
name|name
argument_list|,
name|temp
index|[
literal|1
index|]
operator|.
name|mode
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|path1
index|[
literal|1
index|]
index|[
literal|0
index|]
condition|)
name|printf
argument_list|(
literal|"Deleted: %s\n"
argument_list|,
name|name
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|strcmp
argument_list|(
name|temp
index|[
literal|0
index|]
operator|.
name|mode
argument_list|,
name|temp
index|[
literal|1
index|]
operator|.
name|mode
argument_list|)
condition|)
block|{
name|printf
argument_list|(
literal|"Mode changed: %s (%s->%s)\n"
argument_list|,
name|name
argument_list|,
name|temp
index|[
literal|0
index|]
operator|.
name|mode
argument_list|,
name|temp
index|[
literal|1
index|]
operator|.
name|mode
argument_list|)
expr_stmt|;
comment|/* Be careful.  We do not want to diff between 		 * symlink and a file. 		 */
if|if
condition|(
name|strncmp
argument_list|(
name|temp
index|[
literal|0
index|]
operator|.
name|mode
argument_list|,
literal|"120"
argument_list|,
literal|3
argument_list|)
operator|!=
name|strncmp
argument_list|(
name|temp
index|[
literal|1
index|]
operator|.
name|mode
argument_list|,
literal|"120"
argument_list|,
literal|3
argument_list|)
condition|)
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
name|fflush
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
name|execlp
argument_list|(
literal|"/bin/sh"
argument_list|,
literal|"sh"
argument_list|,
literal|"-c"
argument_list|,
name|cmd
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * Given a name and sha1 pair, if the dircache tells us the file in  * the work tree has that object contents, return true, so that  * prepare_temp_file() does not have to inflate and extract.  */
end_comment
begin_function
DECL|function|work_tree_matches
specifier|static
name|int
name|work_tree_matches
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|cache_entry
modifier|*
name|ce
decl_stmt|;
name|struct
name|stat
name|st
decl_stmt|;
name|int
name|pos
decl_stmt|,
name|len
decl_stmt|;
comment|/* We do not read the cache ourselves here, because the 	 * benchmark with my previous version that always reads cache 	 * shows that it makes things worse for diff-tree comparing 	 * two linux-2.6 kernel trees in an already checked out work 	 * tree.  This is because most diff-tree comparison deals with 	 * only a small number of files, while reading the cache is 	 * expensive for a large project, and its cost outweighs the 	 * savings we get by not inflating the object to a temporary 	 * file.  Practically, this code only helps when we are used 	 * by diff-cache --cached, which does read the cache before 	 * calling us. 	 */
if|if
condition|(
operator|!
name|active_cache
condition|)
return|return
literal|0
return|;
name|len
operator|=
name|strlen
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|pos
operator|=
name|cache_name_pos
argument_list|(
name|name
argument_list|,
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
operator|<
literal|0
condition|)
return|return
literal|0
return|;
name|ce
operator|=
name|active_cache
index|[
name|pos
index|]
expr_stmt|;
if|if
condition|(
operator|(
name|lstat
argument_list|(
name|name
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
operator|)
operator|||
operator|!
name|S_ISREG
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
operator|||
name|cache_match_stat
argument_list|(
name|ce
argument_list|,
operator|&
name|st
argument_list|)
operator|||
name|memcmp
argument_list|(
name|sha1
argument_list|,
name|ce
operator|->
name|sha1
argument_list|,
literal|20
argument_list|)
condition|)
return|return
literal|0
return|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|prep_temp_blob
specifier|static
name|void
name|prep_temp_blob
parameter_list|(
name|struct
name|diff_tempfile
modifier|*
name|temp
parameter_list|,
name|void
modifier|*
name|blob
parameter_list|,
name|unsigned
name|long
name|size
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
name|int
name|fd
decl_stmt|;
name|strcpy
argument_list|(
name|temp
operator|->
name|tmp_path
argument_list|,
literal|".diff_XXXXXX"
argument_list|)
expr_stmt|;
name|fd
operator|=
name|mkstemp
argument_list|(
name|temp
operator|->
name|tmp_path
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
literal|"unable to create temp-file"
argument_list|)
expr_stmt|;
if|if
condition|(
name|write
argument_list|(
name|fd
argument_list|,
name|blob
argument_list|,
name|size
argument_list|)
operator|!=
name|size
condition|)
name|die
argument_list|(
literal|"unable to write temp-file"
argument_list|)
expr_stmt|;
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|temp
operator|->
name|name
operator|=
name|temp
operator|->
name|tmp_path
expr_stmt|;
name|strcpy
argument_list|(
name|temp
operator|->
name|hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|temp
operator|->
name|hex
index|[
literal|40
index|]
operator|=
literal|0
expr_stmt|;
name|sprintf
argument_list|(
name|temp
operator|->
name|mode
argument_list|,
literal|"%06o"
argument_list|,
name|mode
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|prepare_temp_file
specifier|static
name|void
name|prepare_temp_file
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|diff_tempfile
modifier|*
name|temp
parameter_list|,
name|struct
name|diff_spec
modifier|*
name|one
parameter_list|)
block|{
specifier|static
name|unsigned
name|char
name|null_sha1
index|[
literal|20
index|]
init|=
block|{
literal|0
block|, }
decl_stmt|;
name|int
name|use_work_tree
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|!
name|one
operator|->
name|file_valid
condition|)
block|{
name|not_a_valid_file
label|:
comment|/* A '-' entry produces this for file-2, and 		 * a '+' entry produces this for file-1. 		 */
name|temp
operator|->
name|name
operator|=
literal|"/dev/null"
expr_stmt|;
name|strcpy
argument_list|(
name|temp
operator|->
name|hex
argument_list|,
literal|"."
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|temp
operator|->
name|mode
argument_list|,
literal|"."
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|one
operator|->
name|sha1_valid
operator|&&
operator|(
operator|!
name|memcmp
argument_list|(
name|one
operator|->
name|blob_sha1
argument_list|,
name|null_sha1
argument_list|,
sizeof|sizeof
argument_list|(
name|null_sha1
argument_list|)
argument_list|)
operator|||
name|work_tree_matches
argument_list|(
name|name
argument_list|,
name|one
operator|->
name|blob_sha1
argument_list|)
operator|)
condition|)
name|use_work_tree
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|one
operator|->
name|sha1_valid
operator|||
name|use_work_tree
condition|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
name|temp
operator|->
name|name
operator|=
name|name
expr_stmt|;
if|if
condition|(
name|lstat
argument_list|(
name|temp
operator|->
name|name
argument_list|,
operator|&
name|st
argument_list|)
operator|<
literal|0
condition|)
block|{
if|if
condition|(
name|errno
operator|==
name|ENOENT
condition|)
goto|goto
name|not_a_valid_file
goto|;
name|die
argument_list|(
literal|"stat(%s): %s"
argument_list|,
name|temp
operator|->
name|name
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
name|S_ISLNK
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
condition|)
block|{
name|int
name|ret
decl_stmt|;
name|char
modifier|*
name|buf
decl_stmt|,
name|buf_
index|[
literal|1024
index|]
decl_stmt|;
name|buf
operator|=
operator|(
operator|(
sizeof|sizeof
argument_list|(
name|buf_
argument_list|)
operator|<
name|st
operator|.
name|st_size
operator|)
condition|?
name|xmalloc
argument_list|(
name|st
operator|.
name|st_size
argument_list|)
else|:
name|buf_
operator|)
expr_stmt|;
name|ret
operator|=
name|readlink
argument_list|(
name|name
argument_list|,
name|buf
argument_list|,
name|st
operator|.
name|st_size
argument_list|)
expr_stmt|;
if|if
condition|(
name|ret
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"readlink(%s)"
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|prep_temp_blob
argument_list|(
name|temp
argument_list|,
name|buf
argument_list|,
name|st
operator|.
name|st_size
argument_list|,
operator|(
name|one
operator|->
name|sha1_valid
condition|?
name|one
operator|->
name|blob_sha1
else|:
name|null_sha1
operator|)
argument_list|,
operator|(
name|one
operator|->
name|sha1_valid
condition|?
name|one
operator|->
name|mode
else|:
name|S_IFLNK
operator|)
argument_list|)
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
operator|!
name|one
operator|->
name|sha1_valid
condition|)
name|strcpy
argument_list|(
name|temp
operator|->
name|hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|null_sha1
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|strcpy
argument_list|(
name|temp
operator|->
name|hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|one
operator|->
name|blob_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|sprintf
argument_list|(
name|temp
operator|->
name|mode
argument_list|,
literal|"%06o"
argument_list|,
name|S_IFREG
operator||
name|ce_permissions
argument_list|(
name|st
operator|.
name|st_mode
argument_list|)
argument_list|)
expr_stmt|;
block|}
return|return;
block|}
else|else
block|{
name|void
modifier|*
name|blob
decl_stmt|;
name|char
name|type
index|[
literal|20
index|]
decl_stmt|;
name|unsigned
name|long
name|size
decl_stmt|;
name|blob
operator|=
name|read_sha1_file
argument_list|(
name|one
operator|->
name|blob_sha1
argument_list|,
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|blob
operator|||
name|strcmp
argument_list|(
name|type
argument_list|,
literal|"blob"
argument_list|)
condition|)
name|die
argument_list|(
literal|"unable to read blob object for %s (%s)"
argument_list|,
name|name
argument_list|,
name|sha1_to_hex
argument_list|(
name|one
operator|->
name|blob_sha1
argument_list|)
argument_list|)
expr_stmt|;
name|prep_temp_blob
argument_list|(
name|temp
argument_list|,
name|blob
argument_list|,
name|size
argument_list|,
name|one
operator|->
name|blob_sha1
argument_list|,
name|one
operator|->
name|mode
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|blob
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|remove_tempfile
specifier|static
name|void
name|remove_tempfile
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
literal|2
condition|;
name|i
operator|++
control|)
if|if
condition|(
name|diff_temp
index|[
name|i
index|]
operator|.
name|name
operator|==
name|diff_temp
index|[
name|i
index|]
operator|.
name|tmp_path
condition|)
block|{
name|unlink
argument_list|(
name|diff_temp
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
name|diff_temp
index|[
name|i
index|]
operator|.
name|name
operator|=
name|NULL
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|remove_tempfile_on_signal
specifier|static
name|void
name|remove_tempfile_on_signal
parameter_list|(
name|int
name|signo
parameter_list|)
block|{
name|remove_tempfile
argument_list|()
expr_stmt|;
block|}
end_function
begin_comment
comment|/* An external diff command takes:  *  * diff-cmd name infile1 infile1-sha1 infile1-mode \  *               infile2 infile2-sha1 infile2-mode.  *  */
end_comment
begin_function
DECL|function|run_external_diff
name|void
name|run_external_diff
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|diff_spec
modifier|*
name|one
parameter_list|,
name|struct
name|diff_spec
modifier|*
name|two
parameter_list|)
block|{
name|struct
name|diff_tempfile
modifier|*
name|temp
init|=
name|diff_temp
decl_stmt|;
name|pid_t
name|pid
decl_stmt|;
name|int
name|status
decl_stmt|;
specifier|static
name|int
name|atexit_asked
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|one
operator|&&
name|two
condition|)
block|{
name|prepare_temp_file
argument_list|(
name|name
argument_list|,
operator|&
name|temp
index|[
literal|0
index|]
argument_list|,
name|one
argument_list|)
expr_stmt|;
name|prepare_temp_file
argument_list|(
name|name
argument_list|,
operator|&
name|temp
index|[
literal|1
index|]
argument_list|,
name|two
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|atexit_asked
operator|&&
operator|(
name|temp
index|[
literal|0
index|]
operator|.
name|name
operator|==
name|temp
index|[
literal|0
index|]
operator|.
name|tmp_path
operator|||
name|temp
index|[
literal|1
index|]
operator|.
name|name
operator|==
name|temp
index|[
literal|1
index|]
operator|.
name|tmp_path
operator|)
condition|)
block|{
name|atexit_asked
operator|=
literal|1
expr_stmt|;
name|atexit
argument_list|(
name|remove_tempfile
argument_list|)
expr_stmt|;
block|}
name|signal
argument_list|(
name|SIGINT
argument_list|,
name|remove_tempfile_on_signal
argument_list|)
expr_stmt|;
block|}
name|fflush
argument_list|(
name|NULL
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
literal|"unable to fork"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|pid
condition|)
block|{
specifier|const
name|char
modifier|*
name|pgm
init|=
name|external_diff
argument_list|()
decl_stmt|;
if|if
condition|(
name|pgm
condition|)
block|{
if|if
condition|(
name|one
operator|&&
name|two
condition|)
name|execlp
argument_list|(
name|pgm
argument_list|,
name|pgm
argument_list|,
name|name
argument_list|,
name|temp
index|[
literal|0
index|]
operator|.
name|name
argument_list|,
name|temp
index|[
literal|0
index|]
operator|.
name|hex
argument_list|,
name|temp
index|[
literal|0
index|]
operator|.
name|mode
argument_list|,
name|temp
index|[
literal|1
index|]
operator|.
name|name
argument_list|,
name|temp
index|[
literal|1
index|]
operator|.
name|hex
argument_list|,
name|temp
index|[
literal|1
index|]
operator|.
name|mode
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
else|else
name|execlp
argument_list|(
name|pgm
argument_list|,
name|pgm
argument_list|,
name|name
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
comment|/* 		 * otherwise we use the built-in one. 		 */
if|if
condition|(
name|one
operator|&&
name|two
condition|)
name|builtin_diff
argument_list|(
name|name
argument_list|,
name|temp
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"* Unmerged path %s\n"
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|0
argument_list|)
expr_stmt|;
block|}
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
operator|||
operator|!
name|WIFEXITED
argument_list|(
name|status
argument_list|)
operator|||
name|WEXITSTATUS
argument_list|(
name|status
argument_list|)
condition|)
block|{
comment|/* Earlier we did not check the exit status because 		 * diff exits non-zero if files are different, and 		 * we are not interested in knowing that.  It was a 		 * mistake which made it harder to quit a diff-* 		 * session that uses the git-apply-patch-script as 		 * the GIT_EXTERNAL_DIFF.  A custom GIT_EXTERNAL_DIFF 		 * should also exit non-zero only when it wants to 		 * abort the entire diff-* session. 		 */
name|remove_tempfile
argument_list|()
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"external diff died, stopping at %s.\n"
argument_list|,
name|name
argument_list|)
expr_stmt|;
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
block|}
name|remove_tempfile
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|diff_addremove
name|void
name|diff_addremove
parameter_list|(
name|int
name|addremove
parameter_list|,
name|unsigned
name|mode
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|char
name|concatpath
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|struct
name|diff_spec
name|spec
index|[
literal|2
index|]
decl_stmt|,
modifier|*
name|one
decl_stmt|,
modifier|*
name|two
decl_stmt|;
name|memcpy
argument_list|(
name|spec
index|[
literal|0
index|]
operator|.
name|blob_sha1
argument_list|,
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|spec
index|[
literal|0
index|]
operator|.
name|mode
operator|=
name|mode
expr_stmt|;
name|spec
index|[
literal|0
index|]
operator|.
name|sha1_valid
operator|=
name|spec
index|[
literal|0
index|]
operator|.
name|file_valid
operator|=
literal|1
expr_stmt|;
name|spec
index|[
literal|1
index|]
operator|.
name|file_valid
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|addremove
operator|==
literal|'+'
condition|)
block|{
name|one
operator|=
name|spec
operator|+
literal|1
expr_stmt|;
name|two
operator|=
name|spec
expr_stmt|;
block|}
else|else
block|{
name|one
operator|=
name|spec
expr_stmt|;
name|two
operator|=
name|one
operator|+
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|path
condition|)
block|{
name|strcpy
argument_list|(
name|concatpath
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|concatpath
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
name|run_external_diff
argument_list|(
name|path
condition|?
name|concatpath
else|:
name|base
argument_list|,
name|one
argument_list|,
name|two
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|diff_change
name|void
name|diff_change
parameter_list|(
name|unsigned
name|old_mode
parameter_list|,
name|unsigned
name|new_mode
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|old_sha1
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|new_sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|char
name|concatpath
index|[
name|PATH_MAX
index|]
decl_stmt|;
name|struct
name|diff_spec
name|spec
index|[
literal|2
index|]
decl_stmt|;
name|memcpy
argument_list|(
name|spec
index|[
literal|0
index|]
operator|.
name|blob_sha1
argument_list|,
name|old_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|spec
index|[
literal|0
index|]
operator|.
name|mode
operator|=
name|old_mode
expr_stmt|;
name|memcpy
argument_list|(
name|spec
index|[
literal|1
index|]
operator|.
name|blob_sha1
argument_list|,
name|new_sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|spec
index|[
literal|1
index|]
operator|.
name|mode
operator|=
name|new_mode
expr_stmt|;
name|spec
index|[
literal|0
index|]
operator|.
name|sha1_valid
operator|=
name|spec
index|[
literal|0
index|]
operator|.
name|file_valid
operator|=
literal|1
expr_stmt|;
name|spec
index|[
literal|1
index|]
operator|.
name|sha1_valid
operator|=
name|spec
index|[
literal|1
index|]
operator|.
name|file_valid
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|path
condition|)
block|{
name|strcpy
argument_list|(
name|concatpath
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|strcat
argument_list|(
name|concatpath
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
name|run_external_diff
argument_list|(
name|path
condition|?
name|concatpath
else|:
name|base
argument_list|,
operator|&
name|spec
index|[
literal|0
index|]
argument_list|,
operator|&
name|spec
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|diff_unmerge
name|void
name|diff_unmerge
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|)
block|{
name|run_external_diff
argument_list|(
name|path
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
