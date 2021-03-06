begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"lockfile.h"
end_include
begin_include
include|#
directive|include
file|"bundle.h"
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
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"revision.h"
end_include
begin_include
include|#
directive|include
file|"list-objects.h"
end_include
begin_include
include|#
directive|include
file|"run-command.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"argv-array.h"
end_include
begin_decl_stmt
DECL|variable|bundle_signature
specifier|static
specifier|const
name|char
name|bundle_signature
index|[]
init|=
literal|"# v2 git bundle\n"
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|add_to_ref_list
specifier|static
name|void
name|add_to_ref_list
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
specifier|const
name|char
modifier|*
name|name
parameter_list|,
name|struct
name|ref_list
modifier|*
name|list
parameter_list|)
block|{
name|ALLOC_GROW
argument_list|(
name|list
operator|->
name|list
argument_list|,
name|list
operator|->
name|nr
operator|+
literal|1
argument_list|,
name|list
operator|->
name|alloc
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|list
operator|->
name|list
index|[
name|list
operator|->
name|nr
index|]
operator|.
name|sha1
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|list
operator|->
name|list
index|[
name|list
operator|->
name|nr
index|]
operator|.
name|name
operator|=
name|xstrdup
argument_list|(
name|name
argument_list|)
expr_stmt|;
name|list
operator|->
name|nr
operator|++
expr_stmt|;
block|}
end_function
begin_function
DECL|function|parse_bundle_header
specifier|static
name|int
name|parse_bundle_header
parameter_list|(
name|int
name|fd
parameter_list|,
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|,
specifier|const
name|char
modifier|*
name|report_path
parameter_list|)
block|{
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|int
name|status
init|=
literal|0
decl_stmt|;
comment|/* The bundle header begins with the signature */
if|if
condition|(
name|strbuf_getwholeline_fd
argument_list|(
operator|&
name|buf
argument_list|,
name|fd
argument_list|,
literal|'\n'
argument_list|)
operator|||
name|strcmp
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
name|bundle_signature
argument_list|)
condition|)
block|{
if|if
condition|(
name|report_path
condition|)
name|error
argument_list|(
name|_
argument_list|(
literal|"'%s' does not look like a v2 bundle file"
argument_list|)
argument_list|,
name|report_path
argument_list|)
expr_stmt|;
name|status
operator|=
operator|-
literal|1
expr_stmt|;
goto|goto
name|abort
goto|;
block|}
comment|/* The bundle header ends with an empty line */
while|while
condition|(
operator|!
name|strbuf_getwholeline_fd
argument_list|(
operator|&
name|buf
argument_list|,
name|fd
argument_list|,
literal|'\n'
argument_list|)
operator|&&
name|buf
operator|.
name|len
operator|&&
name|buf
operator|.
name|buf
index|[
literal|0
index|]
operator|!=
literal|'\n'
condition|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|is_prereq
init|=
literal|0
decl_stmt|;
if|if
condition|(
operator|*
name|buf
operator|.
name|buf
operator|==
literal|'-'
condition|)
block|{
name|is_prereq
operator|=
literal|1
expr_stmt|;
name|strbuf_remove
argument_list|(
operator|&
name|buf
argument_list|,
literal|0
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
name|strbuf_rtrim
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
comment|/* 		 * Tip lines have object name, SP, and refname. 		 * Prerequisites have object name that is optionally 		 * followed by SP and subject line. 		 */
if|if
condition|(
name|get_sha1_hex
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
name|sha1
argument_list|)
operator|||
operator|(
name|buf
operator|.
name|len
operator|>
literal|40
operator|&&
operator|!
name|isspace
argument_list|(
name|buf
operator|.
name|buf
index|[
literal|40
index|]
argument_list|)
operator|)
operator|||
operator|(
operator|!
name|is_prereq
operator|&&
name|buf
operator|.
name|len
operator|<=
literal|40
operator|)
condition|)
block|{
if|if
condition|(
name|report_path
condition|)
name|error
argument_list|(
name|_
argument_list|(
literal|"unrecognized header: %s%s (%d)"
argument_list|)
argument_list|,
operator|(
name|is_prereq
condition|?
literal|"-"
else|:
literal|""
operator|)
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
operator|(
name|int
operator|)
name|buf
operator|.
name|len
argument_list|)
expr_stmt|;
name|status
operator|=
operator|-
literal|1
expr_stmt|;
break|break;
block|}
else|else
block|{
if|if
condition|(
name|is_prereq
condition|)
name|add_to_ref_list
argument_list|(
name|sha1
argument_list|,
literal|""
argument_list|,
operator|&
name|header
operator|->
name|prerequisites
argument_list|)
expr_stmt|;
else|else
name|add_to_ref_list
argument_list|(
name|sha1
argument_list|,
name|buf
operator|.
name|buf
operator|+
literal|41
argument_list|,
operator|&
name|header
operator|->
name|references
argument_list|)
expr_stmt|;
block|}
block|}
name|abort
label|:
if|if
condition|(
name|status
condition|)
block|{
name|close
argument_list|(
name|fd
argument_list|)
expr_stmt|;
name|fd
operator|=
operator|-
literal|1
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
return|return
name|fd
return|;
block|}
end_function
begin_function
DECL|function|read_bundle_header
name|int
name|read_bundle_header
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|)
block|{
name|int
name|fd
init|=
name|open
argument_list|(
name|path
argument_list|,
name|O_RDONLY
argument_list|)
decl_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"could not open '%s'"
argument_list|)
argument_list|,
name|path
argument_list|)
return|;
return|return
name|parse_bundle_header
argument_list|(
name|fd
argument_list|,
name|header
argument_list|,
name|path
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|is_bundle
name|int
name|is_bundle
parameter_list|(
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|quiet
parameter_list|)
block|{
name|struct
name|bundle_header
name|header
decl_stmt|;
name|int
name|fd
init|=
name|open
argument_list|(
name|path
argument_list|,
name|O_RDONLY
argument_list|)
decl_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
literal|0
return|;
name|memset
argument_list|(
operator|&
name|header
argument_list|,
literal|0
argument_list|,
sizeof|sizeof
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
name|fd
operator|=
name|parse_bundle_header
argument_list|(
name|fd
argument_list|,
operator|&
name|header
argument_list|,
name|quiet
condition|?
name|NULL
else|:
name|path
argument_list|)
expr_stmt|;
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
return|return
operator|(
name|fd
operator|>=
literal|0
operator|)
return|;
block|}
end_function
begin_function
DECL|function|list_refs
specifier|static
name|int
name|list_refs
parameter_list|(
name|struct
name|ref_list
modifier|*
name|r
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
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
name|r
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
block|{
name|int
name|j
decl_stmt|;
for|for
control|(
name|j
operator|=
literal|1
init|;
name|j
operator|<
name|argc
condition|;
name|j
operator|++
control|)
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|r
operator|->
name|list
index|[
name|i
index|]
operator|.
name|name
argument_list|,
name|argv
index|[
name|j
index|]
argument_list|)
condition|)
break|break;
if|if
condition|(
name|j
operator|==
name|argc
condition|)
continue|continue;
block|}
name|printf
argument_list|(
literal|"%s %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|r
operator|->
name|list
index|[
name|i
index|]
operator|.
name|sha1
argument_list|)
argument_list|,
name|r
operator|->
name|list
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/* Remember to update object flag allocation in object.h */
end_comment
begin_define
DECL|macro|PREREQ_MARK
define|#
directive|define
name|PREREQ_MARK
value|(1u<<16)
end_define
begin_function
DECL|function|verify_bundle
name|int
name|verify_bundle
parameter_list|(
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|,
name|int
name|verbose
parameter_list|)
block|{
comment|/* 	 * Do fast check, then if any prereqs are missing then go line by line 	 * to be verbose about the errors 	 */
name|struct
name|ref_list
modifier|*
name|p
init|=
operator|&
name|header
operator|->
name|prerequisites
decl_stmt|;
name|struct
name|rev_info
name|revs
decl_stmt|;
specifier|const
name|char
modifier|*
name|argv
index|[]
init|=
block|{
name|NULL
block|,
literal|"--all"
block|,
name|NULL
block|}
decl_stmt|;
name|struct
name|object_array
name|refs
decl_stmt|;
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
name|int
name|i
decl_stmt|,
name|ret
init|=
literal|0
decl_stmt|,
name|req_nr
decl_stmt|;
specifier|const
name|char
modifier|*
name|message
init|=
name|_
argument_list|(
literal|"Repository lacks these prerequisite commits:"
argument_list|)
decl_stmt|;
name|init_revisions
argument_list|(
operator|&
name|revs
argument_list|,
name|NULL
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
name|p
operator|->
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|ref_list_entry
modifier|*
name|e
init|=
name|p
operator|->
name|list
operator|+
name|i
decl_stmt|;
name|struct
name|object
modifier|*
name|o
init|=
name|parse_object
argument_list|(
name|e
operator|->
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|o
condition|)
block|{
name|o
operator|->
name|flags
operator||=
name|PREREQ_MARK
expr_stmt|;
name|add_pending_object
argument_list|(
operator|&
name|revs
argument_list|,
name|o
argument_list|,
name|e
operator|->
name|name
argument_list|)
expr_stmt|;
continue|continue;
block|}
if|if
condition|(
operator|++
name|ret
operator|==
literal|1
condition|)
name|error
argument_list|(
literal|"%s"
argument_list|,
name|message
argument_list|)
expr_stmt|;
name|error
argument_list|(
literal|"%s %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|e
operator|->
name|sha1
argument_list|)
argument_list|,
name|e
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|revs
operator|.
name|pending
operator|.
name|nr
operator|!=
name|p
operator|->
name|nr
condition|)
return|return
name|ret
return|;
name|req_nr
operator|=
name|revs
operator|.
name|pending
operator|.
name|nr
expr_stmt|;
name|setup_revisions
argument_list|(
literal|2
argument_list|,
name|argv
argument_list|,
operator|&
name|revs
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|refs
operator|=
name|revs
operator|.
name|pending
expr_stmt|;
name|revs
operator|.
name|leak_pending
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|prepare_revision_walk
argument_list|(
operator|&
name|revs
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"revision walk setup failed"
argument_list|)
argument_list|)
expr_stmt|;
name|i
operator|=
name|req_nr
expr_stmt|;
while|while
condition|(
name|i
operator|&&
operator|(
name|commit
operator|=
name|get_revision
argument_list|(
operator|&
name|revs
argument_list|)
operator|)
condition|)
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|PREREQ_MARK
condition|)
name|i
operator|--
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|req_nr
condition|;
name|i
operator|++
control|)
if|if
condition|(
operator|!
operator|(
name|refs
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|item
operator|->
name|flags
operator|&
name|SHOWN
operator|)
condition|)
block|{
if|if
condition|(
operator|++
name|ret
operator|==
literal|1
condition|)
name|error
argument_list|(
literal|"%s"
argument_list|,
name|message
argument_list|)
expr_stmt|;
name|error
argument_list|(
literal|"%s %s"
argument_list|,
name|oid_to_hex
argument_list|(
operator|&
name|refs
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|item
operator|->
name|oid
argument_list|)
argument_list|,
name|refs
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
block|}
name|clear_commit_marks_for_object_array
argument_list|(
operator|&
name|refs
argument_list|,
name|ALL_REV_FLAGS
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|refs
operator|.
name|objects
argument_list|)
expr_stmt|;
if|if
condition|(
name|verbose
condition|)
block|{
name|struct
name|ref_list
modifier|*
name|r
decl_stmt|;
name|r
operator|=
operator|&
name|header
operator|->
name|references
expr_stmt|;
name|printf_ln
argument_list|(
name|Q_
argument_list|(
literal|"The bundle contains this ref:"
argument_list|,
literal|"The bundle contains these %d refs:"
argument_list|,
name|r
operator|->
name|nr
argument_list|)
argument_list|,
name|r
operator|->
name|nr
argument_list|)
expr_stmt|;
name|list_refs
argument_list|(
name|r
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|r
operator|=
operator|&
name|header
operator|->
name|prerequisites
expr_stmt|;
if|if
condition|(
operator|!
name|r
operator|->
name|nr
condition|)
block|{
name|printf_ln
argument_list|(
name|_
argument_list|(
literal|"The bundle records a complete history."
argument_list|)
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|printf_ln
argument_list|(
name|Q_
argument_list|(
literal|"The bundle requires this ref:"
argument_list|,
literal|"The bundle requires these %d refs:"
argument_list|,
name|r
operator|->
name|nr
argument_list|)
argument_list|,
name|r
operator|->
name|nr
argument_list|)
expr_stmt|;
name|list_refs
argument_list|(
name|r
argument_list|,
literal|0
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
block|}
block|}
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|list_bundle_refs
name|int
name|list_bundle_refs
parameter_list|(
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
return|return
name|list_refs
argument_list|(
operator|&
name|header
operator|->
name|references
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|is_tag_in_date_range
specifier|static
name|int
name|is_tag_in_date_range
parameter_list|(
name|struct
name|object
modifier|*
name|tag
parameter_list|,
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|)
block|{
name|unsigned
name|long
name|size
decl_stmt|;
name|enum
name|object_type
name|type
decl_stmt|;
name|char
modifier|*
name|buf
init|=
name|NULL
decl_stmt|,
modifier|*
name|line
decl_stmt|,
modifier|*
name|lineend
decl_stmt|;
name|unsigned
name|long
name|date
decl_stmt|;
name|int
name|result
init|=
literal|1
decl_stmt|;
if|if
condition|(
name|revs
operator|->
name|max_age
operator|==
operator|-
literal|1
operator|&&
name|revs
operator|->
name|min_age
operator|==
operator|-
literal|1
condition|)
goto|goto
name|out
goto|;
name|buf
operator|=
name|read_sha1_file
argument_list|(
name|tag
operator|->
name|oid
operator|.
name|hash
argument_list|,
operator|&
name|type
argument_list|,
operator|&
name|size
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|buf
condition|)
goto|goto
name|out
goto|;
name|line
operator|=
name|memmem
argument_list|(
name|buf
argument_list|,
name|size
argument_list|,
literal|"\ntagger "
argument_list|,
literal|8
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|line
operator|++
condition|)
goto|goto
name|out
goto|;
name|lineend
operator|=
name|memchr
argument_list|(
name|line
argument_list|,
literal|'\n'
argument_list|,
name|buf
operator|+
name|size
operator|-
name|line
argument_list|)
expr_stmt|;
name|line
operator|=
name|memchr
argument_list|(
name|line
argument_list|,
literal|'>'
argument_list|,
name|lineend
condition|?
name|lineend
operator|-
name|line
else|:
name|buf
operator|+
name|size
operator|-
name|line
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|line
operator|++
condition|)
goto|goto
name|out
goto|;
name|date
operator|=
name|strtoul
argument_list|(
name|line
argument_list|,
name|NULL
argument_list|,
literal|10
argument_list|)
expr_stmt|;
name|result
operator|=
operator|(
name|revs
operator|->
name|max_age
operator|==
operator|-
literal|1
operator|||
name|revs
operator|->
name|max_age
operator|<
name|date
operator|)
operator|&&
operator|(
name|revs
operator|->
name|min_age
operator|==
operator|-
literal|1
operator|||
name|revs
operator|->
name|min_age
operator|>
name|date
operator|)
expr_stmt|;
name|out
label|:
name|free
argument_list|(
name|buf
argument_list|)
expr_stmt|;
return|return
name|result
return|;
block|}
end_function
begin_comment
comment|/* Write the pack data to bundle_fd, then close it if it is> 1. */
end_comment
begin_function
DECL|function|write_pack_data
specifier|static
name|int
name|write_pack_data
parameter_list|(
name|int
name|bundle_fd
parameter_list|,
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|)
block|{
name|struct
name|child_process
name|pack_objects
init|=
name|CHILD_PROCESS_INIT
decl_stmt|;
name|int
name|i
decl_stmt|;
name|argv_array_pushl
argument_list|(
operator|&
name|pack_objects
operator|.
name|args
argument_list|,
literal|"pack-objects"
argument_list|,
literal|"--all-progress-implied"
argument_list|,
literal|"--stdout"
argument_list|,
literal|"--thin"
argument_list|,
literal|"--delta-base-offset"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|pack_objects
operator|.
name|in
operator|=
operator|-
literal|1
expr_stmt|;
name|pack_objects
operator|.
name|out
operator|=
name|bundle_fd
expr_stmt|;
name|pack_objects
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|pack_objects
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"Could not spawn pack-objects"
argument_list|)
argument_list|)
return|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|revs
operator|->
name|pending
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object
modifier|*
name|object
init|=
name|revs
operator|->
name|pending
operator|.
name|objects
index|[
name|i
index|]
operator|.
name|item
decl_stmt|;
if|if
condition|(
name|object
operator|->
name|flags
operator|&
name|UNINTERESTING
condition|)
name|write_or_die
argument_list|(
name|pack_objects
operator|.
name|in
argument_list|,
literal|"^"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|pack_objects
operator|.
name|in
argument_list|,
name|oid_to_hex
argument_list|(
operator|&
name|object
operator|->
name|oid
argument_list|)
argument_list|,
name|GIT_SHA1_HEXSZ
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|pack_objects
operator|.
name|in
argument_list|,
literal|"\n"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
name|close
argument_list|(
name|pack_objects
operator|.
name|in
argument_list|)
expr_stmt|;
if|if
condition|(
name|finish_command
argument_list|(
operator|&
name|pack_objects
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"pack-objects died"
argument_list|)
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|compute_and_write_prerequisites
specifier|static
name|int
name|compute_and_write_prerequisites
parameter_list|(
name|int
name|bundle_fd
parameter_list|,
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
parameter_list|)
block|{
name|struct
name|child_process
name|rls
init|=
name|CHILD_PROCESS_INIT
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
name|FILE
modifier|*
name|rls_fout
decl_stmt|;
name|int
name|i
decl_stmt|;
name|argv_array_pushl
argument_list|(
operator|&
name|rls
operator|.
name|args
argument_list|,
literal|"rev-list"
argument_list|,
literal|"--boundary"
argument_list|,
literal|"--pretty=oneline"
argument_list|,
name|NULL
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
condition|;
name|i
operator|++
control|)
name|argv_array_push
argument_list|(
operator|&
name|rls
operator|.
name|args
argument_list|,
name|argv
index|[
name|i
index|]
argument_list|)
expr_stmt|;
name|rls
operator|.
name|out
operator|=
operator|-
literal|1
expr_stmt|;
name|rls
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|start_command
argument_list|(
operator|&
name|rls
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|rls_fout
operator|=
name|xfdopen
argument_list|(
name|rls
operator|.
name|out
argument_list|,
literal|"r"
argument_list|)
expr_stmt|;
while|while
condition|(
name|strbuf_getwholeline
argument_list|(
operator|&
name|buf
argument_list|,
name|rls_fout
argument_list|,
literal|'\n'
argument_list|)
operator|!=
name|EOF
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
name|buf
operator|.
name|len
operator|>
literal|0
operator|&&
name|buf
operator|.
name|buf
index|[
literal|0
index|]
operator|==
literal|'-'
condition|)
block|{
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
name|buf
operator|.
name|buf
argument_list|,
name|buf
operator|.
name|len
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|get_sha1_hex
argument_list|(
name|buf
operator|.
name|buf
operator|+
literal|1
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|struct
name|object
modifier|*
name|object
init|=
name|parse_object_or_die
argument_list|(
name|sha1
argument_list|,
name|buf
operator|.
name|buf
argument_list|)
decl_stmt|;
name|object
operator|->
name|flags
operator||=
name|UNINTERESTING
expr_stmt|;
name|add_pending_object
argument_list|(
name|revs
argument_list|,
name|object
argument_list|,
name|buf
operator|.
name|buf
argument_list|)
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
operator|!
name|get_sha1_hex
argument_list|(
name|buf
operator|.
name|buf
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
name|struct
name|object
modifier|*
name|object
init|=
name|parse_object_or_die
argument_list|(
name|sha1
argument_list|,
name|buf
operator|.
name|buf
argument_list|)
decl_stmt|;
name|object
operator|->
name|flags
operator||=
name|SHOWN
expr_stmt|;
block|}
block|}
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
name|fclose
argument_list|(
name|rls_fout
argument_list|)
expr_stmt|;
if|if
condition|(
name|finish_command
argument_list|(
operator|&
name|rls
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"rev-list died"
argument_list|)
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/*  * Write out bundle refs based on the tips already  * parsed into revs.pending. As a side effect, may  * manipulate revs.pending to include additional  * necessary objects (like tags).  *  * Returns the number of refs written, or negative  * on error.  */
end_comment
begin_function
DECL|function|write_bundle_refs
specifier|static
name|int
name|write_bundle_refs
parameter_list|(
name|int
name|bundle_fd
parameter_list|,
name|struct
name|rev_info
modifier|*
name|revs
parameter_list|)
block|{
name|int
name|i
decl_stmt|;
name|int
name|ref_count
init|=
literal|0
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|revs
operator|->
name|pending
operator|.
name|nr
condition|;
name|i
operator|++
control|)
block|{
name|struct
name|object_array_entry
modifier|*
name|e
init|=
name|revs
operator|->
name|pending
operator|.
name|objects
operator|+
name|i
decl_stmt|;
name|struct
name|object_id
name|oid
decl_stmt|;
name|char
modifier|*
name|ref
decl_stmt|;
specifier|const
name|char
modifier|*
name|display_ref
decl_stmt|;
name|int
name|flag
decl_stmt|;
if|if
condition|(
name|e
operator|->
name|item
operator|->
name|flags
operator|&
name|UNINTERESTING
condition|)
continue|continue;
if|if
condition|(
name|dwim_ref
argument_list|(
name|e
operator|->
name|name
argument_list|,
name|strlen
argument_list|(
name|e
operator|->
name|name
argument_list|)
argument_list|,
name|oid
operator|.
name|hash
argument_list|,
operator|&
name|ref
argument_list|)
operator|!=
literal|1
condition|)
goto|goto
name|skip_write_ref
goto|;
if|if
condition|(
name|read_ref_full
argument_list|(
name|e
operator|->
name|name
argument_list|,
name|RESOLVE_REF_READING
argument_list|,
name|oid
operator|.
name|hash
argument_list|,
operator|&
name|flag
argument_list|)
condition|)
name|flag
operator|=
literal|0
expr_stmt|;
name|display_ref
operator|=
operator|(
name|flag
operator|&
name|REF_ISSYMREF
operator|)
condition|?
name|e
operator|->
name|name
else|:
name|ref
expr_stmt|;
if|if
condition|(
name|e
operator|->
name|item
operator|->
name|type
operator|==
name|OBJ_TAG
operator|&&
operator|!
name|is_tag_in_date_range
argument_list|(
name|e
operator|->
name|item
argument_list|,
name|revs
argument_list|)
condition|)
block|{
name|e
operator|->
name|item
operator|->
name|flags
operator||=
name|UNINTERESTING
expr_stmt|;
goto|goto
name|skip_write_ref
goto|;
block|}
comment|/* 		 * Make sure the refs we wrote out is correct; --max-count and 		 * other limiting options could have prevented all the tips 		 * from getting output. 		 * 		 * Non commit objects such as tags and blobs do not have 		 * this issue as they are not affected by those extra 		 * constraints. 		 */
if|if
condition|(
operator|!
operator|(
name|e
operator|->
name|item
operator|->
name|flags
operator|&
name|SHOWN
operator|)
operator|&&
name|e
operator|->
name|item
operator|->
name|type
operator|==
name|OBJ_COMMIT
condition|)
block|{
name|warning
argument_list|(
name|_
argument_list|(
literal|"ref '%s' is excluded by the rev-list options"
argument_list|)
argument_list|,
name|e
operator|->
name|name
argument_list|)
expr_stmt|;
goto|goto
name|skip_write_ref
goto|;
block|}
comment|/* 		 * If you run "git bundle create bndl v1.0..v2.0", the 		 * name of the positive ref is "v2.0" but that is the 		 * commit that is referenced by the tag, and not the tag 		 * itself. 		 */
if|if
condition|(
name|oidcmp
argument_list|(
operator|&
name|oid
argument_list|,
operator|&
name|e
operator|->
name|item
operator|->
name|oid
argument_list|)
condition|)
block|{
comment|/* 			 * Is this the positive end of a range expressed 			 * in terms of a tag (e.g. v2.0 from the range 			 * "v1.0..v2.0")? 			 */
name|struct
name|commit
modifier|*
name|one
init|=
name|lookup_commit_reference
argument_list|(
name|oid
operator|.
name|hash
argument_list|)
decl_stmt|;
name|struct
name|object
modifier|*
name|obj
decl_stmt|;
if|if
condition|(
name|e
operator|->
name|item
operator|==
operator|&
operator|(
name|one
operator|->
name|object
operator|)
condition|)
block|{
comment|/* 				 * Need to include e->name as an 				 * independent ref to the pack-objects 				 * input, so that the tag is included 				 * in the output; otherwise we would 				 * end up triggering "empty bundle" 				 * error. 				 */
name|obj
operator|=
name|parse_object_or_die
argument_list|(
name|oid
operator|.
name|hash
argument_list|,
name|e
operator|->
name|name
argument_list|)
expr_stmt|;
name|obj
operator|->
name|flags
operator||=
name|SHOWN
expr_stmt|;
name|add_pending_object
argument_list|(
name|revs
argument_list|,
name|obj
argument_list|,
name|e
operator|->
name|name
argument_list|)
expr_stmt|;
block|}
goto|goto
name|skip_write_ref
goto|;
block|}
name|ref_count
operator|++
expr_stmt|;
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
name|oid_to_hex
argument_list|(
operator|&
name|e
operator|->
name|item
operator|->
name|oid
argument_list|)
argument_list|,
literal|40
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
literal|" "
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
name|display_ref
argument_list|,
name|strlen
argument_list|(
name|display_ref
argument_list|)
argument_list|)
expr_stmt|;
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
literal|"\n"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
name|skip_write_ref
label|:
name|free
argument_list|(
name|ref
argument_list|)
expr_stmt|;
block|}
comment|/* end header */
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
literal|"\n"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
return|return
name|ref_count
return|;
block|}
end_function
begin_function
DECL|function|create_bundle
name|int
name|create_bundle
parameter_list|(
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|,
specifier|const
name|char
modifier|*
name|path
parameter_list|,
name|int
name|argc
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|argv
parameter_list|)
block|{
specifier|static
name|struct
name|lock_file
name|lock
decl_stmt|;
name|int
name|bundle_fd
init|=
operator|-
literal|1
decl_stmt|;
name|int
name|bundle_to_stdout
decl_stmt|;
name|int
name|ref_count
init|=
literal|0
decl_stmt|;
name|struct
name|rev_info
name|revs
decl_stmt|;
name|bundle_to_stdout
operator|=
operator|!
name|strcmp
argument_list|(
name|path
argument_list|,
literal|"-"
argument_list|)
expr_stmt|;
if|if
condition|(
name|bundle_to_stdout
condition|)
name|bundle_fd
operator|=
literal|1
expr_stmt|;
else|else
block|{
name|bundle_fd
operator|=
name|hold_lock_file_for_update
argument_list|(
operator|&
name|lock
argument_list|,
name|path
argument_list|,
name|LOCK_DIE_ON_ERROR
argument_list|)
expr_stmt|;
comment|/* 		 * write_pack_data() will close the fd passed to it, 		 * but commit_lock_file() will also try to close the 		 * lockfile's fd. So make a copy of the file 		 * descriptor to avoid trying to close it twice. 		 */
name|bundle_fd
operator|=
name|dup
argument_list|(
name|bundle_fd
argument_list|)
expr_stmt|;
if|if
condition|(
name|bundle_fd
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"unable to dup file descriptor"
argument_list|)
expr_stmt|;
block|}
comment|/* write signature */
name|write_or_die
argument_list|(
name|bundle_fd
argument_list|,
name|bundle_signature
argument_list|,
name|strlen
argument_list|(
name|bundle_signature
argument_list|)
argument_list|)
expr_stmt|;
comment|/* init revs to list objects for pack-objects later */
name|save_commit_buffer
operator|=
literal|0
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|revs
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
comment|/* write prerequisites */
if|if
condition|(
name|compute_and_write_prerequisites
argument_list|(
name|bundle_fd
argument_list|,
operator|&
name|revs
argument_list|,
name|argc
argument_list|,
name|argv
argument_list|)
condition|)
goto|goto
name|err
goto|;
name|argc
operator|=
name|setup_revisions
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
operator|&
name|revs
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|>
literal|1
condition|)
block|{
name|error
argument_list|(
name|_
argument_list|(
literal|"unrecognized argument: %s"
argument_list|)
argument_list|,
name|argv
index|[
literal|1
index|]
argument_list|)
expr_stmt|;
goto|goto
name|err
goto|;
block|}
name|object_array_remove_duplicates
argument_list|(
operator|&
name|revs
operator|.
name|pending
argument_list|)
expr_stmt|;
name|ref_count
operator|=
name|write_bundle_refs
argument_list|(
name|bundle_fd
argument_list|,
operator|&
name|revs
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|ref_count
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Refusing to create empty bundle."
argument_list|)
argument_list|)
expr_stmt|;
elseif|else
if|if
condition|(
name|ref_count
operator|<
literal|0
condition|)
goto|goto
name|err
goto|;
comment|/* write pack */
if|if
condition|(
name|write_pack_data
argument_list|(
name|bundle_fd
argument_list|,
operator|&
name|revs
argument_list|)
condition|)
block|{
name|bundle_fd
operator|=
operator|-
literal|1
expr_stmt|;
comment|/* already closed by the above call */
goto|goto
name|err
goto|;
block|}
if|if
condition|(
operator|!
name|bundle_to_stdout
condition|)
block|{
if|if
condition|(
name|commit_lock_file
argument_list|(
operator|&
name|lock
argument_list|)
condition|)
name|die_errno
argument_list|(
name|_
argument_list|(
literal|"cannot create '%s'"
argument_list|)
argument_list|,
name|path
argument_list|)
expr_stmt|;
block|}
return|return
literal|0
return|;
name|err
label|:
if|if
condition|(
operator|!
name|bundle_to_stdout
condition|)
block|{
if|if
condition|(
literal|0
operator|<=
name|bundle_fd
condition|)
name|close
argument_list|(
name|bundle_fd
argument_list|)
expr_stmt|;
name|rollback_lock_file
argument_list|(
operator|&
name|lock
argument_list|)
expr_stmt|;
block|}
return|return
operator|-
literal|1
return|;
block|}
end_function
begin_function
DECL|function|unbundle
name|int
name|unbundle
parameter_list|(
name|struct
name|bundle_header
modifier|*
name|header
parameter_list|,
name|int
name|bundle_fd
parameter_list|,
name|int
name|flags
parameter_list|)
block|{
specifier|const
name|char
modifier|*
name|argv_index_pack
index|[]
init|=
block|{
literal|"index-pack"
block|,
literal|"--fix-thin"
block|,
literal|"--stdin"
block|,
name|NULL
block|,
name|NULL
block|}
decl_stmt|;
name|struct
name|child_process
name|ip
init|=
name|CHILD_PROCESS_INIT
decl_stmt|;
if|if
condition|(
name|flags
operator|&
name|BUNDLE_VERBOSE
condition|)
name|argv_index_pack
index|[
literal|3
index|]
operator|=
literal|"-v"
expr_stmt|;
if|if
condition|(
name|verify_bundle
argument_list|(
name|header
argument_list|,
literal|0
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|ip
operator|.
name|argv
operator|=
name|argv_index_pack
expr_stmt|;
name|ip
operator|.
name|in
operator|=
name|bundle_fd
expr_stmt|;
name|ip
operator|.
name|no_stdout
operator|=
literal|1
expr_stmt|;
name|ip
operator|.
name|git_cmd
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|run_command
argument_list|(
operator|&
name|ip
argument_list|)
condition|)
return|return
name|error
argument_list|(
name|_
argument_list|(
literal|"index-pack died"
argument_list|)
argument_list|)
return|;
return|return
literal|0
return|;
block|}
end_function
end_unit
