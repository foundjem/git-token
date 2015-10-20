begin_unit
begin_comment
comment|/*  * "diff --no-index" support  * Copyright (c) 2007 by Johannes Schindelin  * Copyright (c) 2008 by Junio C Hamano  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"color.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"diffcore.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"log-tree.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"string-list.h"
end_include
begin_include
include|#
directive|include
file|"dir.h"
end_include
begin_function
DECL|function|read_directory_contents
specifier|static
name|int
name|read_directory_contents
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|string_list
modifier|*
name|list
parameter_list|)
block|{
name|DIR
modifier|*
name|dir
decl_stmt|;
name|struct
name|dirent
modifier|*
name|e
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|dir
operator|=
name|opendir
argument_list|(
name|path
argument_list|)
operator|)
condition|)
return|return
name|error
argument_list|(
literal|"Could not open directory %s"
argument_list|,
name|path
argument_list|)
return|;
while|while
condition|(
operator|(
name|e
operator|=
name|readdir
argument_list|(
name|dir
argument_list|)
operator|)
condition|)
if|if
condition|(
operator|!
name|is_dot_or_dotdot
argument_list|(
name|e
operator|->
name|d_name
argument_list|)
condition|)
name|string_list_insert
argument_list|(
name|list
argument_list|,
name|e
operator|->
name|d_name
argument_list|)
expr_stmt|;
name|closedir
argument_list|(
name|dir
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * This should be "(standard input)" or something, but it will  * probably expose many more breakages in the way no-index code  * is bolted onto the diff callchain.  */
end_comment
begin_decl_stmt
DECL|variable|file_from_standard_input
specifier|static
specifier|const
name|char
name|file_from_standard_input
index|[]
init|=
literal|"-"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|get_mode
specifier|static
name|int
name|get_mode
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
modifier|*
name|mode
parameter_list|)
block|{
name|struct
name|stat
name|st
decl_stmt|;
if|if
condition|(
operator|!
name|path
operator|||
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"/dev/null"
argument_list|)
condition|)
operator|*
name|mode
operator|=
literal|0
expr_stmt|;
ifdef|#
directive|ifdef
name|GIT_WINDOWS_NATIVE
elseif|else
if|if
condition|(
operator|!
name|strcasecmp
argument_list|(
name|path
argument_list|,
literal|"nul"
argument_list|)
condition|)
operator|*
name|mode
operator|=
literal|0
expr_stmt|;
endif|#
directive|endif
elseif|else
if|if
condition|(
name|path
operator|==
name|file_from_standard_input
condition|)
operator|*
name|mode
operator|=
name|create_ce_mode
argument_list|(
literal|0666
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|lstat
argument_list|(
name|path
argument_list|,
operator|&
name|st
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Could not access '%s'"
argument_list|,
name|path
argument_list|)
return|;
else|else
operator|*
name|mode
operator|=
name|st
operator|.
name|st_mode
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|populate_from_stdin
specifier|static
name|int
name|populate_from_stdin
parameter_list|(
name|struct
name|diff_filespec
modifier|*
name|s
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|size_t
name|size
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|strbuf_read
argument_list|(
operator|&
name|buf
argument_list|,
literal|0
argument_list|,
literal|0
argument_list|)
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
literal|"error while reading from stdin %s"
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
name|s
operator|->
name|should_munmap
operator|=
literal|0
expr_stmt|;
name|s
operator|->
name|data
operator|=
name|strbuf_detach
argument_list|(
operator|&
name|buf
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
name|s
operator|->
name|size
operator|=
name|size
expr_stmt|;
name|s
operator|->
name|should_free
operator|=
literal|1
expr_stmt|;
name|s
operator|->
name|is_stdin
operator|=
literal|1
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|noindex_filespec
specifier|static
name|struct
name|diff_filespec
modifier|*
name|noindex_filespec
parameter_list|(
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|int
name|mode
parameter_list|)
block|{
name|struct
name|diff_filespec
modifier|*
name|s
decl_stmt|;
if|if
condition|(
operator|!
name|name
condition|)
name|name
operator|=
literal|"/dev/null"
expr_stmt|;
name|s
operator|=
name|alloc_filespec
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|fill_filespec
argument_list|(
name|s
argument_list|,
name|null_sha1
argument_list|,
literal|0
argument_list|,
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|name
operator|==
name|file_from_standard_input
condition|)
name|populate_from_stdin
argument_list|(
name|s
argument_list|)
expr_stmt|;
return|return
name|s
return|;
block|}
end_function
begin_function
DECL|function|queue_diff
specifier|static
name|int
name|queue_diff
parameter_list|(
name|struct
name|diff_options
modifier|*
name|o
parameter_list|,
specifier|const
name|char
modifier|*
name|name1
parameter_list|,
specifier|const
name|char
modifier|*
name|name2
parameter_list|)
block|{
name|int
name|mode1
init|=
literal|0
decl_stmt|,
name|mode2
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|get_mode
argument_list|(
name|name1
argument_list|,
operator|&
name|mode1
argument_list|)
operator|||
name|get_mode
argument_list|(
name|name2
argument_list|,
operator|&
name|mode2
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|mode1
operator|&&
name|mode2
operator|&&
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
operator|!=
name|S_ISDIR
argument_list|(
name|mode2
argument_list|)
condition|)
block|{
name|struct
name|diff_filespec
modifier|*
name|d1
decl_stmt|,
modifier|*
name|d2
decl_stmt|;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
condition|)
block|{
comment|/* 2 is file that is created */
name|d1
operator|=
name|noindex_filespec
argument_list|(
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|d2
operator|=
name|noindex_filespec
argument_list|(
name|name2
argument_list|,
name|mode2
argument_list|)
expr_stmt|;
name|name2
operator|=
name|NULL
expr_stmt|;
name|mode2
operator|=
literal|0
expr_stmt|;
block|}
else|else
block|{
comment|/* 1 is file that is deleted */
name|d1
operator|=
name|noindex_filespec
argument_list|(
name|name1
argument_list|,
name|mode1
argument_list|)
expr_stmt|;
name|d2
operator|=
name|noindex_filespec
argument_list|(
name|NULL
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|name1
operator|=
name|NULL
expr_stmt|;
name|mode1
operator|=
literal|0
expr_stmt|;
block|}
comment|/* emit that file */
name|diff_queue
argument_list|(
operator|&
name|diff_queued_diff
argument_list|,
name|d1
argument_list|,
name|d2
argument_list|)
expr_stmt|;
comment|/* and then let the entire directory be created or deleted */
block|}
if|if
condition|(
name|S_ISDIR
argument_list|(
name|mode1
argument_list|)
operator|||
name|S_ISDIR
argument_list|(
name|mode2
argument_list|)
condition|)
block|{
name|struct
name|strbuf
name|buffer1
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|buffer2
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|string_list
name|p1
init|=
name|STRING_LIST_INIT_DUP
decl_stmt|;
name|struct
name|string_list
name|p2
init|=
name|STRING_LIST_INIT_DUP
decl_stmt|;
name|int
name|i1
decl_stmt|,
name|i2
decl_stmt|,
name|ret
init|=
literal|0
decl_stmt|;
name|size_t
name|len1
init|=
literal|0
decl_stmt|,
name|len2
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|name1
operator|&&
name|read_directory_contents
argument_list|(
name|name1
argument_list|,
operator|&
name|p1
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|name2
operator|&&
name|read_directory_contents
argument_list|(
name|name2
argument_list|,
operator|&
name|p2
argument_list|)
condition|)
block|{
name|string_list_clear
argument_list|(
operator|&
name|p1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
if|if
condition|(
name|name1
condition|)
block|{
name|strbuf_addstr
argument_list|(
operator|&
name|buffer1
argument_list|,
name|name1
argument_list|)
expr_stmt|;
name|strbuf_complete
argument_list|(
operator|&
name|buffer1
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
name|len1
operator|=
name|buffer1
operator|.
name|len
expr_stmt|;
block|}
if|if
condition|(
name|name2
condition|)
block|{
name|strbuf_addstr
argument_list|(
operator|&
name|buffer2
argument_list|,
name|name2
argument_list|)
expr_stmt|;
name|strbuf_complete
argument_list|(
operator|&
name|buffer2
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
name|len2
operator|=
name|buffer2
operator|.
name|len
expr_stmt|;
block|}
for|for
control|(
name|i1
operator|=
name|i2
operator|=
literal|0
init|;
operator|!
name|ret
operator|&&
operator|(
name|i1
operator|<
name|p1
operator|.
name|nr
operator|||
name|i2
operator|<
name|p2
operator|.
name|nr
operator|)
condition|;
control|)
block|{
specifier|const
name|char
modifier|*
name|n1
decl_stmt|,
modifier|*
name|n2
decl_stmt|;
name|int
name|comp
decl_stmt|;
name|strbuf_setlen
argument_list|(
operator|&
name|buffer1
argument_list|,
name|len1
argument_list|)
expr_stmt|;
name|strbuf_setlen
argument_list|(
operator|&
name|buffer2
argument_list|,
name|len2
argument_list|)
expr_stmt|;
if|if
condition|(
name|i1
operator|==
name|p1
operator|.
name|nr
condition|)
name|comp
operator|=
literal|1
expr_stmt|;
elseif|else
if|if
condition|(
name|i2
operator|==
name|p2
operator|.
name|nr
condition|)
name|comp
operator|=
operator|-
literal|1
expr_stmt|;
else|else
name|comp
operator|=
name|strcmp
argument_list|(
name|p1
operator|.
name|items
index|[
name|i1
index|]
operator|.
name|string
argument_list|,
name|p2
operator|.
name|items
index|[
name|i2
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
if|if
condition|(
name|comp
operator|>
literal|0
condition|)
name|n1
operator|=
name|NULL
expr_stmt|;
else|else
block|{
name|strbuf_addstr
argument_list|(
operator|&
name|buffer1
argument_list|,
name|p1
operator|.
name|items
index|[
name|i1
operator|++
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
name|n1
operator|=
name|buffer1
operator|.
name|buf
expr_stmt|;
block|}
if|if
condition|(
name|comp
operator|<
literal|0
condition|)
name|n2
operator|=
name|NULL
expr_stmt|;
else|else
block|{
name|strbuf_addstr
argument_list|(
operator|&
name|buffer2
argument_list|,
name|p2
operator|.
name|items
index|[
name|i2
operator|++
index|]
operator|.
name|string
argument_list|)
expr_stmt|;
name|n2
operator|=
name|buffer2
operator|.
name|buf
expr_stmt|;
block|}
name|ret
operator|=
name|queue_diff
argument_list|(
name|o
argument_list|,
name|n1
argument_list|,
name|n2
argument_list|)
expr_stmt|;
block|}
name|string_list_clear
argument_list|(
operator|&
name|p1
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|string_list_clear
argument_list|(
operator|&
name|p2
argument_list|,
literal|0
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buffer1
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|buffer2
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
else|else
block|{
name|struct
name|diff_filespec
modifier|*
name|d1
decl_stmt|,
modifier|*
name|d2
decl_stmt|;
if|if
condition|(
name|DIFF_OPT_TST
argument_list|(
name|o
argument_list|,
name|REVERSE_DIFF
argument_list|)
condition|)
block|{
name|unsigned
name|tmp
decl_stmt|;
specifier|const
name|char
modifier|*
name|tmp_c
decl_stmt|;
name|tmp
operator|=
name|mode1
expr_stmt|;
name|mode1
operator|=
name|mode2
expr_stmt|;
name|mode2
operator|=
name|tmp
expr_stmt|;
name|tmp_c
operator|=
name|name1
expr_stmt|;
name|name1
operator|=
name|name2
expr_stmt|;
name|name2
operator|=
name|tmp_c
expr_stmt|;
block|}
name|d1
operator|=
name|noindex_filespec
argument_list|(
name|name1
argument_list|,
name|mode1
argument_list|)
expr_stmt|;
name|d2
operator|=
name|noindex_filespec
argument_list|(
name|name2
argument_list|,
name|mode2
argument_list|)
expr_stmt|;
name|diff_queue
argument_list|(
operator|&
name|diff_queued_diff
argument_list|,
name|d1
argument_list|,
name|d2
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
block|}
end_function
begin_comment
comment|/* append basename of F to D */
end_comment
begin_function
DECL|function|append_basename
specifier|static
name|void
name|append_basename
parameter_list|(
name|struct
name|strbuf
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|dir
parameter_list|,
specifier|const
name|char
modifier|*
name|file
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|tail
init|=
name|strrchr
argument_list|(
name|file
argument_list|,
literal|'/'
argument_list|)
decl_stmt|;
name|strbuf_addstr
argument_list|(
name|path
argument_list|,
name|dir
argument_list|)
expr_stmt|;
while|while
condition|(
name|path
operator|->
name|len
operator|&&
name|path
operator|->
name|buf
index|[
name|path
operator|->
name|len
operator|-
literal|1
index|]
operator|==
literal|'/'
condition|)
name|path
operator|->
name|len
operator|--
expr_stmt|;
name|strbuf_addch
argument_list|(
name|path
argument_list|,
literal|'/'
argument_list|)
expr_stmt|;
name|strbuf_addstr
argument_list|(
name|path
argument_list|,
name|tail
condition|?
name|tail
operator|+
literal|1
else|:
name|file
argument_list|)
expr_stmt|;
block|}
end_function
begin_comment
comment|/*  * DWIM "diff D F" into "diff D/F F" and "diff F D" into "diff F D/F"  * Note that we append the basename of F to D/, so "diff a/b/file D"  * becomes "diff a/b/file D/file", not "diff a/b/file D/a/b/file".  */
end_comment
begin_function
DECL|function|fixup_paths
specifier|static
name|void
name|fixup_paths
parameter_list|(
specifier|const
name|char
modifier|*
modifier|*
name|path
parameter_list|,
name|struct
name|strbuf
modifier|*
name|replacement
parameter_list|)
block|{
name|unsigned
name|int
name|isdir0
decl_stmt|,
name|isdir1
decl_stmt|;
if|if
condition|(
name|path
index|[
literal|0
index|]
operator|==
name|file_from_standard_input
operator|||
name|path
index|[
literal|1
index|]
operator|==
name|file_from_standard_input
condition|)
return|return;
name|isdir0
operator|=
name|is_directory
argument_list|(
name|path
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|isdir1
operator|=
name|is_directory
argument_list|(
name|path
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|isdir0
operator|==
name|isdir1
condition|)
return|return;
if|if
condition|(
name|isdir0
condition|)
block|{
name|append_basename
argument_list|(
name|replacement
argument_list|,
name|path
index|[
literal|0
index|]
argument_list|,
name|path
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
name|path
index|[
literal|0
index|]
operator|=
name|replacement
operator|->
name|buf
expr_stmt|;
block|}
else|else
block|{
name|append_basename
argument_list|(
name|replacement
argument_list|,
name|path
index|[
literal|1
index|]
argument_list|,
name|path
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|path
index|[
literal|1
index|]
operator|=
name|replacement
operator|->
name|buf
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|diff_no_index
name|void
name|diff_no_index
parameter_list|(
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|,
specifier|const
name|char
modifier|*
name|prefix
parameter_list|)
block|{
name|int
name|i
decl_stmt|,
name|prefixlen
decl_stmt|;
specifier|const
name|char
modifier|*
name|paths
index|[
literal|2
index|]
decl_stmt|;
name|struct
name|strbuf
name|replacement
init|=
name|STRBUF_INIT
decl_stmt|;
name|diff_setup
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
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
operator|-
literal|2
condition|;
control|)
block|{
name|int
name|j
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--no-index"
argument_list|)
condition|)
name|i
operator|++
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|argv
index|[
name|i
index|]
argument_list|,
literal|"--"
argument_list|)
condition|)
name|i
operator|++
expr_stmt|;
else|else
block|{
name|j
operator|=
name|diff_opt_parse
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|argv
operator|+
name|i
argument_list|,
name|argc
operator|-
name|i
argument_list|)
expr_stmt|;
if|if
condition|(
name|j
operator|<=
literal|0
condition|)
name|die
argument_list|(
literal|"invalid diff option/value: %s"
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|i
operator|+=
name|j
expr_stmt|;
block|}
block|}
name|prefixlen
operator|=
name|prefix
condition|?
name|strlen
argument_list|(
name|prefix
argument_list|)
else|:
literal|0
expr_stmt|;
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
specifier|const
name|char
modifier|*
name|p
init|=
name|argv
index|[
name|argc
operator|-
literal|2
operator|+
name|i
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|p
argument_list|,
literal|"-"
argument_list|)
condition|)
comment|/* 			 * stdin should be spelled as "-"; if you have 			 * path that is "-", spell it as "./-". 			 */
name|p
operator|=
name|file_from_standard_input
expr_stmt|;
elseif|else
if|if
condition|(
name|prefixlen
condition|)
name|p
operator|=
name|xstrdup
argument_list|(
name|prefix_filename
argument_list|(
name|prefix
argument_list|,
name|prefixlen
argument_list|,
name|p
argument_list|)
argument_list|)
expr_stmt|;
name|paths
index|[
name|i
index|]
operator|=
name|p
expr_stmt|;
block|}
name|fixup_paths
argument_list|(
name|paths
argument_list|,
operator|&
name|replacement
argument_list|)
expr_stmt|;
name|revs
operator|->
name|diffopt
operator|.
name|skip_stat_unmatch
operator|=
literal|1
expr_stmt|;
if|if
condition|(
operator|!
name|revs
operator|->
name|diffopt
operator|.
name|output_format
condition|)
name|revs
operator|->
name|diffopt
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_PATCH
expr_stmt|;
name|DIFF_OPT_SET
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|NO_INDEX
argument_list|)
expr_stmt|;
name|revs
operator|->
name|max_count
operator|=
operator|-
literal|2
expr_stmt|;
name|diff_setup_done
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|setup_diff_pager
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|DIFF_OPT_SET
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|EXIT_WITH_STATUS
argument_list|)
expr_stmt|;
if|if
condition|(
name|queue_diff
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
name|paths
index|[
literal|0
index|]
argument_list|,
name|paths
index|[
literal|1
index|]
argument_list|)
condition|)
name|exit
argument_list|(
literal|1
argument_list|)
expr_stmt|;
name|diff_set_mnemonic_prefix
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
literal|"1/"
argument_list|,
literal|"2/"
argument_list|)
expr_stmt|;
name|diffcore_std
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|replacement
argument_list|)
expr_stmt|;
comment|/* 	 * The return code for --no-index imitates diff(1): 	 * 0 = no changes, 1 = changes, else error 	 */
name|exit
argument_list|(
name|diff_result_code
argument_list|(
operator|&
name|revs
operator|->
name|diffopt
argument_list|,
literal|0
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
