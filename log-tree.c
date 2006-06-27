begin_unit
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
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"log-tree.h"
end_include
begin_function
DECL|function|show_parents
specifier|static
name|void
name|show_parents
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|int
name|abbrev
parameter_list|)
block|{
name|struct
name|commit_list
modifier|*
name|p
decl_stmt|;
for|for
control|(
name|p
operator|=
name|commit
operator|->
name|parents
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
name|struct
name|commit
modifier|*
name|parent
init|=
name|p
operator|->
name|item
decl_stmt|;
name|printf
argument_list|(
literal|" %s"
argument_list|,
name|diff_unique_abbrev
argument_list|(
name|parent
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|abbrev
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|append_signoff
specifier|static
name|int
name|append_signoff
parameter_list|(
name|char
modifier|*
name|buf
parameter_list|,
name|int
name|buf_sz
parameter_list|,
name|int
name|at
parameter_list|,
specifier|const
name|char
modifier|*
name|signoff
parameter_list|)
block|{
name|int
name|signoff_len
init|=
name|strlen
argument_list|(
name|signoff
argument_list|)
decl_stmt|;
specifier|static
specifier|const
name|char
name|signed_off_by
index|[]
init|=
literal|"Signed-off-by: "
decl_stmt|;
name|char
modifier|*
name|cp
init|=
name|buf
decl_stmt|;
comment|/* Do we have enough space to add it? */
if|if
condition|(
name|buf_sz
operator|-
name|at
operator|<=
name|strlen
argument_list|(
name|signed_off_by
argument_list|)
operator|+
name|signoff_len
operator|+
literal|2
condition|)
return|return
name|at
return|;
comment|/* First see if we already have the sign-off by the signer */
while|while
condition|(
literal|1
condition|)
block|{
name|cp
operator|=
name|strstr
argument_list|(
name|cp
argument_list|,
name|signed_off_by
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|cp
condition|)
break|break;
name|cp
operator|+=
name|strlen
argument_list|(
name|signed_off_by
argument_list|)
expr_stmt|;
if|if
condition|(
operator|(
name|cp
operator|+
name|signoff_len
operator|<
name|buf
operator|+
name|at
operator|)
operator|&&
operator|!
name|strncmp
argument_list|(
name|cp
argument_list|,
name|signoff
argument_list|,
name|signoff_len
argument_list|)
operator|&&
name|isspace
argument_list|(
name|cp
index|[
name|signoff_len
index|]
argument_list|)
condition|)
return|return
name|at
return|;
comment|/* we already have him */
block|}
name|strcpy
argument_list|(
name|buf
operator|+
name|at
argument_list|,
name|signed_off_by
argument_list|)
expr_stmt|;
name|at
operator|+=
name|strlen
argument_list|(
name|signed_off_by
argument_list|)
expr_stmt|;
name|strcpy
argument_list|(
name|buf
operator|+
name|at
argument_list|,
name|signoff
argument_list|)
expr_stmt|;
name|at
operator|+=
name|signoff_len
expr_stmt|;
name|buf
index|[
name|at
operator|++
index|]
operator|=
literal|'\n'
expr_stmt|;
name|buf
index|[
name|at
index|]
operator|=
literal|0
expr_stmt|;
return|return
name|at
return|;
block|}
end_function
begin_function
DECL|function|show_log
name|void
name|show_log
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
specifier|const
name|char
modifier|*
name|sep
parameter_list|)
block|{
specifier|static
name|char
name|this_header
index|[
literal|16384
index|]
decl_stmt|;
name|struct
name|log_info
modifier|*
name|log
init|=
name|opt
operator|->
name|loginfo
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
init|=
name|log
operator|->
name|commit
decl_stmt|,
modifier|*
name|parent
init|=
name|log
operator|->
name|parent
decl_stmt|;
name|int
name|abbrev
init|=
name|opt
operator|->
name|diffopt
operator|.
name|abbrev
decl_stmt|;
name|int
name|abbrev_commit
init|=
name|opt
operator|->
name|abbrev_commit
condition|?
name|opt
operator|->
name|abbrev
else|:
literal|40
decl_stmt|;
specifier|const
name|char
modifier|*
name|extra
decl_stmt|;
name|int
name|len
decl_stmt|;
specifier|const
name|char
modifier|*
name|subject
init|=
name|NULL
decl_stmt|,
modifier|*
name|extra_headers
init|=
name|opt
operator|->
name|extra_headers
decl_stmt|;
name|opt
operator|->
name|loginfo
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
operator|!
name|opt
operator|->
name|verbose_header
condition|)
block|{
name|fputs
argument_list|(
name|diff_unique_abbrev
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|abbrev_commit
argument_list|)
argument_list|,
name|stdout
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|parents
condition|)
name|show_parents
argument_list|(
name|commit
argument_list|,
name|abbrev_commit
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
return|return;
block|}
comment|/* 	 * The "oneline" format has several special cases: 	 *  - The pretty-printed commit lacks a newline at the end 	 *    of the buffer, but we do want to make sure that we 	 *    have a newline there. If the separator isn't already 	 *    a newline, add an extra one. 	 *  - unlike other log messages, the one-line format does 	 *    not have an empty line between entries. 	 */
name|extra
operator|=
literal|""
expr_stmt|;
if|if
condition|(
operator|*
name|sep
operator|!=
literal|'\n'
operator|&&
name|opt
operator|->
name|commit_format
operator|==
name|CMIT_FMT_ONELINE
condition|)
name|extra
operator|=
literal|"\n"
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|shown_one
operator|&&
name|opt
operator|->
name|commit_format
operator|!=
name|CMIT_FMT_ONELINE
condition|)
name|putchar
argument_list|(
literal|'\n'
argument_list|)
expr_stmt|;
name|opt
operator|->
name|shown_one
operator|=
literal|1
expr_stmt|;
comment|/* 	 * Print header line of header.. 	 */
if|if
condition|(
name|opt
operator|->
name|commit_format
operator|==
name|CMIT_FMT_EMAIL
condition|)
block|{
name|char
modifier|*
name|sha1
init|=
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|opt
operator|->
name|total
operator|>
literal|0
condition|)
block|{
specifier|static
name|char
name|buffer
index|[
literal|64
index|]
decl_stmt|;
name|snprintf
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
argument_list|,
literal|"Subject: [PATCH %d/%d] "
argument_list|,
name|opt
operator|->
name|nr
argument_list|,
name|opt
operator|->
name|total
argument_list|)
expr_stmt|;
name|subject
operator|=
name|buffer
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|opt
operator|->
name|total
operator|==
literal|0
condition|)
name|subject
operator|=
literal|"Subject: [PATCH] "
expr_stmt|;
else|else
name|subject
operator|=
literal|"Subject: "
expr_stmt|;
name|printf
argument_list|(
literal|"From %s Mon Sep 17 00:00:00 2001\n"
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|mime_boundary
condition|)
block|{
specifier|static
name|char
name|subject_buffer
index|[
literal|1024
index|]
decl_stmt|;
specifier|static
name|char
name|buffer
index|[
literal|1024
index|]
decl_stmt|;
name|snprintf
argument_list|(
name|subject_buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|subject_buffer
argument_list|)
operator|-
literal|1
argument_list|,
literal|"%s"
literal|"MIME-Version: 1.0\n"
literal|"Content-Type: multipart/mixed;\n"
literal|" boundary=\"%s%s\"\n"
literal|"\n"
literal|"This is a multi-part message in MIME "
literal|"format.\n"
literal|"--%s%s\n"
literal|"Content-Type: text/plain; "
literal|"charset=UTF-8; format=fixed\n"
literal|"Content-Transfer-Encoding: 8bit\n\n"
argument_list|,
name|extra_headers
condition|?
name|extra_headers
else|:
literal|""
argument_list|,
name|mime_boundary_leader
argument_list|,
name|opt
operator|->
name|mime_boundary
argument_list|,
name|mime_boundary_leader
argument_list|,
name|opt
operator|->
name|mime_boundary
argument_list|)
expr_stmt|;
name|extra_headers
operator|=
name|subject_buffer
expr_stmt|;
name|snprintf
argument_list|(
name|buffer
argument_list|,
sizeof|sizeof
argument_list|(
name|buffer
argument_list|)
operator|-
literal|1
argument_list|,
literal|"--%s%s\n"
literal|"Content-Type: text/x-patch;\n"
literal|" name=\"%s.diff\"\n"
literal|"Content-Transfer-Encoding: 8bit\n"
literal|"Content-Disposition: inline;\n"
literal|" filename=\"%s.diff\"\n\n"
argument_list|,
name|mime_boundary_leader
argument_list|,
name|opt
operator|->
name|mime_boundary
argument_list|,
name|sha1
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|opt
operator|->
name|diffopt
operator|.
name|stat_sep
operator|=
name|buffer
expr_stmt|;
block|}
block|}
else|else
block|{
name|printf
argument_list|(
literal|"%s%s"
argument_list|,
name|opt
operator|->
name|commit_format
operator|==
name|CMIT_FMT_ONELINE
condition|?
literal|""
else|:
literal|"commit "
argument_list|,
name|diff_unique_abbrev
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|abbrev_commit
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|parents
condition|)
name|show_parents
argument_list|(
name|commit
argument_list|,
name|abbrev_commit
argument_list|)
expr_stmt|;
if|if
condition|(
name|parent
condition|)
name|printf
argument_list|(
literal|" (from %s)"
argument_list|,
name|diff_unique_abbrev
argument_list|(
name|parent
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|abbrev_commit
argument_list|)
argument_list|)
expr_stmt|;
name|putchar
argument_list|(
name|opt
operator|->
name|commit_format
operator|==
name|CMIT_FMT_ONELINE
condition|?
literal|' '
else|:
literal|'\n'
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * And then the pretty-printed message itself 	 */
name|len
operator|=
name|pretty_print_commit
argument_list|(
name|opt
operator|->
name|commit_format
argument_list|,
name|commit
argument_list|,
operator|~
literal|0u
argument_list|,
name|this_header
argument_list|,
sizeof|sizeof
argument_list|(
name|this_header
argument_list|)
argument_list|,
name|abbrev
argument_list|,
name|subject
argument_list|,
name|extra_headers
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|add_signoff
condition|)
name|len
operator|=
name|append_signoff
argument_list|(
name|this_header
argument_list|,
sizeof|sizeof
argument_list|(
name|this_header
argument_list|)
argument_list|,
name|len
argument_list|,
name|opt
operator|->
name|add_signoff
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"%s%s%s"
argument_list|,
name|this_header
argument_list|,
name|extra
argument_list|,
name|sep
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|log_tree_diff_flush
name|int
name|log_tree_diff_flush
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|)
block|{
name|diffcore_std
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
if|if
condition|(
name|diff_queue_is_empty
argument_list|()
condition|)
block|{
name|int
name|saved_fmt
init|=
name|opt
operator|->
name|diffopt
operator|.
name|output_format
decl_stmt|;
name|opt
operator|->
name|diffopt
operator|.
name|output_format
operator|=
name|DIFF_FORMAT_NO_OUTPUT
expr_stmt|;
name|diff_flush
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|opt
operator|->
name|diffopt
operator|.
name|output_format
operator|=
name|saved_fmt
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|opt
operator|->
name|loginfo
operator|&&
operator|!
name|opt
operator|->
name|no_commit_id
condition|)
block|{
comment|/* When showing a verbose header (i.e. log message), 		 * and not in --pretty=oneline format, we would want 		 * an extra newline between the end of log and the 		 * output for readability. 		 */
name|show_log
argument_list|(
name|opt
argument_list|,
name|opt
operator|->
name|diffopt
operator|.
name|msg_sep
argument_list|)
expr_stmt|;
if|if
condition|(
name|opt
operator|->
name|verbose_header
operator|&&
name|opt
operator|->
name|commit_format
operator|!=
name|CMIT_FMT_ONELINE
condition|)
block|{
name|int
name|pch
init|=
name|DIFF_FORMAT_DIFFSTAT
operator||
name|DIFF_FORMAT_PATCH
decl_stmt|;
if|if
condition|(
operator|(
name|pch
operator|&
name|opt
operator|->
name|diffopt
operator|.
name|output_format
operator|)
operator|==
name|pch
condition|)
name|printf
argument_list|(
literal|"---%c"
argument_list|,
name|opt
operator|->
name|diffopt
operator|.
name|line_termination
argument_list|)
expr_stmt|;
else|else
name|putchar
argument_list|(
name|opt
operator|->
name|diffopt
operator|.
name|line_termination
argument_list|)
expr_stmt|;
block|}
block|}
name|diff_flush
argument_list|(
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|diff_root_tree
specifier|static
name|int
name|diff_root_tree
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|new
parameter_list|,
specifier|const
name|char
modifier|*
name|base
parameter_list|)
block|{
name|int
name|retval
decl_stmt|;
name|void
modifier|*
name|tree
decl_stmt|;
name|struct
name|tree_desc
name|empty
decl_stmt|,
name|real
decl_stmt|;
name|tree
operator|=
name|read_object_with_reference
argument_list|(
name|new
argument_list|,
name|tree_type
argument_list|,
operator|&
name|real
operator|.
name|size
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|tree
condition|)
name|die
argument_list|(
literal|"unable to read root tree (%s)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|new
argument_list|)
argument_list|)
expr_stmt|;
name|real
operator|.
name|buf
operator|=
name|tree
expr_stmt|;
name|empty
operator|.
name|buf
operator|=
literal|""
expr_stmt|;
name|empty
operator|.
name|size
operator|=
literal|0
expr_stmt|;
name|retval
operator|=
name|diff_tree
argument_list|(
operator|&
name|empty
argument_list|,
operator|&
name|real
argument_list|,
name|base
argument_list|,
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|tree
argument_list|)
expr_stmt|;
name|log_tree_diff_flush
argument_list|(
name|opt
argument_list|)
expr_stmt|;
return|return
name|retval
return|;
block|}
end_function
begin_function
DECL|function|do_diff_combined
specifier|static
name|int
name|do_diff_combined
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|unsigned
specifier|const
name|char
modifier|*
name|sha1
init|=
name|commit
operator|->
name|object
operator|.
name|sha1
decl_stmt|;
name|diff_tree_combined_merge
argument_list|(
name|sha1
argument_list|,
name|opt
operator|->
name|dense_combined_merges
argument_list|,
name|opt
argument_list|)
expr_stmt|;
return|return
operator|!
name|opt
operator|->
name|loginfo
return|;
block|}
end_function
begin_comment
comment|/*  * Show the diff of a commit.  *  * Return true if we printed any log info messages  */
end_comment
begin_function
DECL|function|log_tree_diff
specifier|static
name|int
name|log_tree_diff
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|struct
name|log_info
modifier|*
name|log
parameter_list|)
block|{
name|int
name|showed_log
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
decl_stmt|;
name|unsigned
specifier|const
name|char
modifier|*
name|sha1
init|=
name|commit
operator|->
name|object
operator|.
name|sha1
decl_stmt|;
if|if
condition|(
operator|!
name|opt
operator|->
name|diff
condition|)
return|return
literal|0
return|;
comment|/* Root commit? */
name|parents
operator|=
name|commit
operator|->
name|parents
expr_stmt|;
if|if
condition|(
operator|!
name|parents
condition|)
block|{
if|if
condition|(
name|opt
operator|->
name|show_root_diff
condition|)
name|diff_root_tree
argument_list|(
name|opt
argument_list|,
name|sha1
argument_list|,
literal|""
argument_list|)
expr_stmt|;
return|return
operator|!
name|opt
operator|->
name|loginfo
return|;
block|}
comment|/* More than one parent? */
if|if
condition|(
name|parents
operator|&&
name|parents
operator|->
name|next
condition|)
block|{
if|if
condition|(
name|opt
operator|->
name|ignore_merges
condition|)
return|return
literal|0
return|;
elseif|else
if|if
condition|(
name|opt
operator|->
name|combine_merges
condition|)
return|return
name|do_diff_combined
argument_list|(
name|opt
argument_list|,
name|commit
argument_list|)
return|;
comment|/* If we show individual diffs, show the parent info */
name|log
operator|->
name|parent
operator|=
name|parents
operator|->
name|item
expr_stmt|;
block|}
name|showed_log
operator|=
literal|0
expr_stmt|;
for|for
control|(
init|;
condition|;
control|)
block|{
name|struct
name|commit
modifier|*
name|parent
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|diff_tree_sha1
argument_list|(
name|parent
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|sha1
argument_list|,
literal|""
argument_list|,
operator|&
name|opt
operator|->
name|diffopt
argument_list|)
expr_stmt|;
name|log_tree_diff_flush
argument_list|(
name|opt
argument_list|)
expr_stmt|;
name|showed_log
operator||=
operator|!
name|opt
operator|->
name|loginfo
expr_stmt|;
comment|/* Set up the log info for the next parent, if any.. */
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
if|if
condition|(
operator|!
name|parents
condition|)
break|break;
name|log
operator|->
name|parent
operator|=
name|parents
operator|->
name|item
expr_stmt|;
name|opt
operator|->
name|loginfo
operator|=
name|log
expr_stmt|;
block|}
return|return
name|showed_log
return|;
block|}
end_function
begin_function
DECL|function|log_tree_commit
name|int
name|log_tree_commit
parameter_list|(
name|struct
name|rev_info
modifier|*
name|opt
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|struct
name|log_info
name|log
decl_stmt|;
name|int
name|shown
decl_stmt|;
name|log
operator|.
name|commit
operator|=
name|commit
expr_stmt|;
name|log
operator|.
name|parent
operator|=
name|NULL
expr_stmt|;
name|opt
operator|->
name|loginfo
operator|=
operator|&
name|log
expr_stmt|;
name|shown
operator|=
name|log_tree_diff
argument_list|(
name|opt
argument_list|,
name|commit
argument_list|,
operator|&
name|log
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|shown
operator|&&
name|opt
operator|->
name|loginfo
operator|&&
name|opt
operator|->
name|always_show_header
condition|)
block|{
name|log
operator|.
name|parent
operator|=
name|NULL
expr_stmt|;
name|show_log
argument_list|(
name|opt
argument_list|,
literal|""
argument_list|)
expr_stmt|;
name|shown
operator|=
literal|1
expr_stmt|;
block|}
name|opt
operator|->
name|loginfo
operator|=
name|NULL
expr_stmt|;
return|return
name|shown
return|;
block|}
end_function
end_unit
