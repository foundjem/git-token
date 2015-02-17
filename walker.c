begin_unit
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"walker.h"
end_include
begin_include
include|#
directive|include
file|"commit.h"
end_include
begin_include
include|#
directive|include
file|"tree.h"
end_include
begin_include
include|#
directive|include
file|"tree-walk.h"
end_include
begin_include
include|#
directive|include
file|"tag.h"
end_include
begin_include
include|#
directive|include
file|"blob.h"
end_include
begin_include
include|#
directive|include
file|"refs.h"
end_include
begin_decl_stmt
DECL|variable|current_commit_sha1
specifier|static
name|unsigned
name|char
name|current_commit_sha1
index|[
literal|20
index|]
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|walker_say
name|void
name|walker_say
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
specifier|const
name|char
modifier|*
name|fmt
parameter_list|,
specifier|const
name|char
modifier|*
name|hex
parameter_list|)
block|{
if|if
condition|(
name|walker
operator|->
name|get_verbosely
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|fmt
argument_list|,
name|hex
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|report_missing
specifier|static
name|void
name|report_missing
parameter_list|(
specifier|const
name|struct
name|object
modifier|*
name|obj
parameter_list|)
block|{
name|char
name|missing_hex
index|[
literal|41
index|]
decl_stmt|;
name|strcpy
argument_list|(
name|missing_hex
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Cannot obtain needed %s %s\n"
argument_list|,
name|obj
operator|->
name|type
condition|?
name|typename
argument_list|(
name|obj
operator|->
name|type
argument_list|)
else|:
literal|"object"
argument_list|,
name|missing_hex
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|is_null_sha1
argument_list|(
name|current_commit_sha1
argument_list|)
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"while processing commit %s.\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|current_commit_sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function_decl
specifier|static
name|int
name|process
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|object
modifier|*
name|obj
parameter_list|)
function_decl|;
end_function_decl
begin_function
DECL|function|process_tree
specifier|static
name|int
name|process_tree
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|tree
modifier|*
name|tree
parameter_list|)
block|{
name|struct
name|tree_desc
name|desc
decl_stmt|;
name|struct
name|name_entry
name|entry
decl_stmt|;
if|if
condition|(
name|parse_tree
argument_list|(
name|tree
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|init_tree_desc
argument_list|(
operator|&
name|desc
argument_list|,
name|tree
operator|->
name|buffer
argument_list|,
name|tree
operator|->
name|size
argument_list|)
expr_stmt|;
while|while
condition|(
name|tree_entry
argument_list|(
operator|&
name|desc
argument_list|,
operator|&
name|entry
argument_list|)
condition|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|NULL
decl_stmt|;
comment|/* submodule commits are not stored in the superproject */
if|if
condition|(
name|S_ISGITLINK
argument_list|(
name|entry
operator|.
name|mode
argument_list|)
condition|)
continue|continue;
if|if
condition|(
name|S_ISDIR
argument_list|(
name|entry
operator|.
name|mode
argument_list|)
condition|)
block|{
name|struct
name|tree
modifier|*
name|tree
init|=
name|lookup_tree
argument_list|(
name|entry
operator|.
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|tree
condition|)
name|obj
operator|=
operator|&
name|tree
operator|->
name|object
expr_stmt|;
block|}
else|else
block|{
name|struct
name|blob
modifier|*
name|blob
init|=
name|lookup_blob
argument_list|(
name|entry
operator|.
name|sha1
argument_list|)
decl_stmt|;
if|if
condition|(
name|blob
condition|)
name|obj
operator|=
operator|&
name|blob
operator|->
name|object
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|obj
operator|||
name|process
argument_list|(
name|walker
argument_list|,
name|obj
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
name|free_tree_buffer
argument_list|(
name|tree
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_comment
comment|/* Remember to update object flag allocation in object.h */
end_comment
begin_define
DECL|macro|COMPLETE
define|#
directive|define
name|COMPLETE
value|(1U<< 0)
end_define
begin_define
DECL|macro|SEEN
define|#
directive|define
name|SEEN
value|(1U<< 1)
end_define
begin_define
DECL|macro|TO_SCAN
define|#
directive|define
name|TO_SCAN
value|(1U<< 2)
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
DECL|function|process_commit
specifier|static
name|int
name|process_commit
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
if|if
condition|(
name|parse_commit
argument_list|(
name|commit
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
while|while
condition|(
name|complete
operator|&&
name|complete
operator|->
name|item
operator|->
name|date
operator|>=
name|commit
operator|->
name|date
condition|)
block|{
name|pop_most_recent_commit
argument_list|(
operator|&
name|complete
argument_list|,
name|COMPLETE
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|commit
operator|->
name|object
operator|.
name|flags
operator|&
name|COMPLETE
condition|)
return|return
literal|0
return|;
name|hashcpy
argument_list|(
name|current_commit_sha1
argument_list|,
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
name|walker_say
argument_list|(
name|walker
argument_list|,
literal|"walk %s\n"
argument_list|,
name|sha1_to_hex
argument_list|(
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|walker
operator|->
name|get_tree
condition|)
block|{
if|if
condition|(
name|process
argument_list|(
name|walker
argument_list|,
operator|&
name|commit
operator|->
name|tree
operator|->
name|object
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
operator|!
name|walker
operator|->
name|get_all
condition|)
name|walker
operator|->
name|get_tree
operator|=
literal|0
expr_stmt|;
block|}
if|if
condition|(
name|walker
operator|->
name|get_history
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|commit
operator|->
name|parents
decl_stmt|;
for|for
control|(
init|;
name|parents
condition|;
name|parents
operator|=
name|parents
operator|->
name|next
control|)
block|{
if|if
condition|(
name|process
argument_list|(
name|walker
argument_list|,
operator|&
name|parents
operator|->
name|item
operator|->
name|object
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|process_tag
specifier|static
name|int
name|process_tag
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|tag
modifier|*
name|tag
parameter_list|)
block|{
if|if
condition|(
name|parse_tag
argument_list|(
name|tag
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
return|return
name|process
argument_list|(
name|walker
argument_list|,
name|tag
operator|->
name|tagged
argument_list|)
return|;
block|}
end_function
begin_decl_stmt
DECL|variable|process_queue
specifier|static
name|struct
name|object_list
modifier|*
name|process_queue
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|process_queue_end
specifier|static
name|struct
name|object_list
modifier|*
modifier|*
name|process_queue_end
init|=
operator|&
name|process_queue
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|process_object
specifier|static
name|int
name|process_object
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|object
modifier|*
name|obj
parameter_list|)
block|{
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_COMMIT
condition|)
block|{
if|if
condition|(
name|process_commit
argument_list|(
name|walker
argument_list|,
operator|(
expr|struct
name|commit
operator|*
operator|)
name|obj
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_TREE
condition|)
block|{
if|if
condition|(
name|process_tree
argument_list|(
name|walker
argument_list|,
operator|(
expr|struct
name|tree
operator|*
operator|)
name|obj
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_BLOB
condition|)
block|{
return|return
literal|0
return|;
block|}
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|OBJ_TAG
condition|)
block|{
if|if
condition|(
name|process_tag
argument_list|(
name|walker
argument_list|,
operator|(
expr|struct
name|tag
operator|*
operator|)
name|obj
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
return|return
literal|0
return|;
block|}
return|return
name|error
argument_list|(
literal|"Unable to determine requirements "
literal|"of type %s for %s"
argument_list|,
name|typename
argument_list|(
name|obj
operator|->
name|type
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|process
specifier|static
name|int
name|process
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|struct
name|object
modifier|*
name|obj
parameter_list|)
block|{
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
name|SEEN
condition|)
return|return
literal|0
return|;
name|obj
operator|->
name|flags
operator||=
name|SEEN
expr_stmt|;
if|if
condition|(
name|has_sha1_file
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
condition|)
block|{
comment|/* We already have it, so we should scan it now. */
name|obj
operator|->
name|flags
operator||=
name|TO_SCAN
expr_stmt|;
block|}
else|else
block|{
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
name|COMPLETE
condition|)
return|return
literal|0
return|;
name|walker
operator|->
name|prefetch
argument_list|(
name|walker
argument_list|,
name|obj
operator|->
name|sha1
argument_list|)
expr_stmt|;
block|}
name|object_list_insert
argument_list|(
name|obj
argument_list|,
name|process_queue_end
argument_list|)
expr_stmt|;
name|process_queue_end
operator|=
operator|&
operator|(
operator|*
name|process_queue_end
operator|)
operator|->
name|next
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|loop
specifier|static
name|int
name|loop
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|)
block|{
name|struct
name|object_list
modifier|*
name|elem
decl_stmt|;
while|while
condition|(
name|process_queue
condition|)
block|{
name|struct
name|object
modifier|*
name|obj
init|=
name|process_queue
operator|->
name|item
decl_stmt|;
name|elem
operator|=
name|process_queue
expr_stmt|;
name|process_queue
operator|=
name|elem
operator|->
name|next
expr_stmt|;
name|free
argument_list|(
name|elem
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|process_queue
condition|)
name|process_queue_end
operator|=
operator|&
name|process_queue
expr_stmt|;
comment|/* If we are not scanning this object, we placed it in 		 * the queue because we needed to fetch it first. 		 */
if|if
condition|(
operator|!
operator|(
name|obj
operator|->
name|flags
operator|&
name|TO_SCAN
operator|)
condition|)
block|{
if|if
condition|(
name|walker
operator|->
name|fetch
argument_list|(
name|walker
argument_list|,
name|obj
operator|->
name|sha1
argument_list|)
condition|)
block|{
name|report_missing
argument_list|(
name|obj
argument_list|)
expr_stmt|;
return|return
operator|-
literal|1
return|;
block|}
block|}
if|if
condition|(
operator|!
name|obj
operator|->
name|type
condition|)
name|parse_object
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|process_object
argument_list|(
name|walker
argument_list|,
name|obj
argument_list|)
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
DECL|function|interpret_target
specifier|static
name|int
name|interpret_target
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|char
modifier|*
name|target
parameter_list|,
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
if|if
condition|(
operator|!
name|get_sha1_hex
argument_list|(
name|target
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
literal|0
return|;
if|if
condition|(
operator|!
name|check_refname_format
argument_list|(
name|target
argument_list|,
literal|0
argument_list|)
condition|)
block|{
name|struct
name|ref
modifier|*
name|ref
init|=
name|alloc_ref
argument_list|(
name|target
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|walker
operator|->
name|fetch_ref
argument_list|(
name|walker
argument_list|,
name|ref
argument_list|)
condition|)
block|{
name|hashcpy
argument_list|(
name|sha1
argument_list|,
name|ref
operator|->
name|old_sha1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|ref
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|free
argument_list|(
name|ref
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
parameter_list|,
name|int
name|flag
parameter_list|,
name|void
modifier|*
name|cb_data
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|commit
init|=
name|lookup_commit_reference_gently
argument_list|(
name|sha1
argument_list|,
literal|1
argument_list|)
decl_stmt|;
if|if
condition|(
name|commit
condition|)
block|{
name|commit
operator|->
name|object
operator|.
name|flags
operator||=
name|COMPLETE
expr_stmt|;
name|commit_list_insert
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
DECL|function|walker_targets_stdin
name|int
name|walker_targets_stdin
parameter_list|(
name|char
modifier|*
modifier|*
modifier|*
name|target
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
modifier|*
name|write_ref
parameter_list|)
block|{
name|int
name|targets
init|=
literal|0
decl_stmt|,
name|targets_alloc
init|=
literal|0
decl_stmt|;
name|struct
name|strbuf
name|buf
init|=
name|STRBUF_INIT
decl_stmt|;
operator|*
name|target
operator|=
name|NULL
expr_stmt|;
operator|*
name|write_ref
operator|=
name|NULL
expr_stmt|;
while|while
condition|(
literal|1
condition|)
block|{
name|char
modifier|*
name|rf_one
init|=
name|NULL
decl_stmt|;
name|char
modifier|*
name|tg_one
decl_stmt|;
if|if
condition|(
name|strbuf_getline
argument_list|(
operator|&
name|buf
argument_list|,
name|stdin
argument_list|,
literal|'\n'
argument_list|)
operator|==
name|EOF
condition|)
break|break;
name|tg_one
operator|=
name|buf
operator|.
name|buf
expr_stmt|;
name|rf_one
operator|=
name|strchr
argument_list|(
name|tg_one
argument_list|,
literal|'\t'
argument_list|)
expr_stmt|;
if|if
condition|(
name|rf_one
condition|)
operator|*
name|rf_one
operator|++
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|targets
operator|>=
name|targets_alloc
condition|)
block|{
name|targets_alloc
operator|=
name|targets_alloc
condition|?
name|targets_alloc
operator|*
literal|2
else|:
literal|64
expr_stmt|;
name|REALLOC_ARRAY
argument_list|(
operator|*
name|target
argument_list|,
name|targets_alloc
argument_list|)
expr_stmt|;
name|REALLOC_ARRAY
argument_list|(
operator|*
name|write_ref
argument_list|,
name|targets_alloc
argument_list|)
expr_stmt|;
block|}
operator|(
operator|*
name|target
operator|)
index|[
name|targets
index|]
operator|=
name|xstrdup
argument_list|(
name|tg_one
argument_list|)
expr_stmt|;
operator|(
operator|*
name|write_ref
operator|)
index|[
name|targets
index|]
operator|=
name|xstrdup_or_null
argument_list|(
name|rf_one
argument_list|)
expr_stmt|;
name|targets
operator|++
expr_stmt|;
block|}
name|strbuf_release
argument_list|(
operator|&
name|buf
argument_list|)
expr_stmt|;
return|return
name|targets
return|;
block|}
end_function
begin_function
DECL|function|walker_targets_free
name|void
name|walker_targets_free
parameter_list|(
name|int
name|targets
parameter_list|,
name|char
modifier|*
modifier|*
name|target
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|write_ref
parameter_list|)
block|{
while|while
condition|(
name|targets
operator|--
condition|)
block|{
name|free
argument_list|(
name|target
index|[
name|targets
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|write_ref
condition|)
name|free
argument_list|(
operator|(
name|char
operator|*
operator|)
name|write_ref
index|[
name|targets
index|]
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|walker_fetch
name|int
name|walker_fetch
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|,
name|int
name|targets
parameter_list|,
name|char
modifier|*
modifier|*
name|target
parameter_list|,
specifier|const
name|char
modifier|*
modifier|*
name|write_ref
parameter_list|,
specifier|const
name|char
modifier|*
name|write_ref_log_details
parameter_list|)
block|{
name|struct
name|strbuf
name|refname
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|strbuf
name|err
init|=
name|STRBUF_INIT
decl_stmt|;
name|struct
name|ref_transaction
modifier|*
name|transaction
init|=
name|NULL
decl_stmt|;
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|xmalloc
argument_list|(
name|targets
operator|*
literal|20
argument_list|)
decl_stmt|;
name|char
modifier|*
name|msg
init|=
name|NULL
decl_stmt|;
name|int
name|i
decl_stmt|,
name|ret
init|=
operator|-
literal|1
decl_stmt|;
name|save_commit_buffer
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|write_ref
condition|)
block|{
name|transaction
operator|=
name|ref_transaction_begin
argument_list|(
operator|&
name|err
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|transaction
condition|)
block|{
name|error
argument_list|(
literal|"%s"
argument_list|,
name|err
operator|.
name|buf
argument_list|)
expr_stmt|;
goto|goto
name|done
goto|;
block|}
block|}
if|if
condition|(
operator|!
name|walker
operator|->
name|get_recover
condition|)
block|{
name|for_each_ref
argument_list|(
name|mark_complete
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|commit_list_sort_by_date
argument_list|(
operator|&
name|complete
argument_list|)
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|targets
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
name|interpret_target
argument_list|(
name|walker
argument_list|,
name|target
index|[
name|i
index|]
argument_list|,
operator|&
name|sha1
index|[
literal|20
operator|*
name|i
index|]
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"Could not interpret response from server '%s' as something to pull"
argument_list|,
name|target
index|[
name|i
index|]
argument_list|)
expr_stmt|;
goto|goto
name|done
goto|;
block|}
if|if
condition|(
name|process
argument_list|(
name|walker
argument_list|,
name|lookup_unknown_object
argument_list|(
operator|&
name|sha1
index|[
literal|20
operator|*
name|i
index|]
argument_list|)
argument_list|)
condition|)
goto|goto
name|done
goto|;
block|}
if|if
condition|(
name|loop
argument_list|(
name|walker
argument_list|)
condition|)
goto|goto
name|done
goto|;
if|if
condition|(
operator|!
name|write_ref
condition|)
block|{
name|ret
operator|=
literal|0
expr_stmt|;
goto|goto
name|done
goto|;
block|}
if|if
condition|(
name|write_ref_log_details
condition|)
block|{
name|msg
operator|=
name|xstrfmt
argument_list|(
literal|"fetch from %s"
argument_list|,
name|write_ref_log_details
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|msg
operator|=
name|NULL
expr_stmt|;
block|}
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|targets
condition|;
name|i
operator|++
control|)
block|{
if|if
condition|(
operator|!
name|write_ref
index|[
name|i
index|]
condition|)
continue|continue;
name|strbuf_reset
argument_list|(
operator|&
name|refname
argument_list|)
expr_stmt|;
name|strbuf_addf
argument_list|(
operator|&
name|refname
argument_list|,
literal|"refs/%s"
argument_list|,
name|write_ref
index|[
name|i
index|]
argument_list|)
expr_stmt|;
if|if
condition|(
name|ref_transaction_update
argument_list|(
name|transaction
argument_list|,
name|refname
operator|.
name|buf
argument_list|,
operator|&
name|sha1
index|[
literal|20
operator|*
name|i
index|]
argument_list|,
name|NULL
argument_list|,
literal|0
argument_list|,
name|msg
condition|?
name|msg
else|:
literal|"fetch (unknown)"
argument_list|,
operator|&
name|err
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"%s"
argument_list|,
name|err
operator|.
name|buf
argument_list|)
expr_stmt|;
goto|goto
name|done
goto|;
block|}
block|}
if|if
condition|(
name|ref_transaction_commit
argument_list|(
name|transaction
argument_list|,
operator|&
name|err
argument_list|)
condition|)
block|{
name|error
argument_list|(
literal|"%s"
argument_list|,
name|err
operator|.
name|buf
argument_list|)
expr_stmt|;
goto|goto
name|done
goto|;
block|}
name|ret
operator|=
literal|0
expr_stmt|;
name|done
label|:
name|ref_transaction_free
argument_list|(
name|transaction
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|msg
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|err
argument_list|)
expr_stmt|;
name|strbuf_release
argument_list|(
operator|&
name|refname
argument_list|)
expr_stmt|;
return|return
name|ret
return|;
block|}
end_function
begin_function
DECL|function|walker_free
name|void
name|walker_free
parameter_list|(
name|struct
name|walker
modifier|*
name|walker
parameter_list|)
block|{
name|walker
operator|->
name|cleanup
argument_list|(
name|walker
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|walker
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
