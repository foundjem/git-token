begin_unit
begin_include
include|#
directive|include
file|"fetch.h"
end_include
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
file|"tree.h"
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
DECL|variable|write_ref
specifier|const
name|char
modifier|*
name|write_ref
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|current_ref
specifier|const
name|unsigned
name|char
modifier|*
name|current_ref
init|=
name|NULL
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_tree
name|int
name|get_tree
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_history
name|int
name|get_history
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_all
name|int
name|get_all
init|=
literal|0
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|get_verbosely
name|int
name|get_verbosely
init|=
literal|0
decl_stmt|;
end_decl_stmt
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
DECL|function|pull_say
name|void
name|pull_say
parameter_list|(
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
name|char
modifier|*
name|what
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|missing
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
name|missing
argument_list|)
argument_list|)
expr_stmt|;
empty_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Cannot obtain needed %s %s\nwhile processing commit %s.\n"
argument_list|,
name|what
argument_list|,
name|missing_hex
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
name|tree
modifier|*
name|tree
parameter_list|)
block|{
name|struct
name|tree_entry_list
modifier|*
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
name|entry
operator|=
name|tree
operator|->
name|entries
expr_stmt|;
name|tree
operator|->
name|entries
operator|=
name|NULL
expr_stmt|;
while|while
condition|(
name|entry
condition|)
block|{
name|struct
name|tree_entry_list
modifier|*
name|next
init|=
name|entry
operator|->
name|next
decl_stmt|;
if|if
condition|(
name|process
argument_list|(
name|entry
operator|->
name|item
operator|.
name|any
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
name|free
argument_list|(
name|entry
argument_list|)
expr_stmt|;
name|entry
operator|=
name|next
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_define
DECL|macro|COMPLETE
define|#
directive|define
name|COMPLETE
value|1U
end_define
begin_define
DECL|macro|TO_FETCH
define|#
directive|define
name|TO_FETCH
value|2U
end_define
begin_define
DECL|macro|TO_SCAN
define|#
directive|define
name|TO_SCAN
value|4U
end_define
begin_define
DECL|macro|SCANNED
define|#
directive|define
name|SCANNED
value|8U
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
name|memcpy
argument_list|(
name|current_commit_sha1
argument_list|,
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
literal|20
argument_list|)
expr_stmt|;
name|pull_say
argument_list|(
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
name|get_tree
condition|)
block|{
if|if
condition|(
name|process
argument_list|(
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
name|get_all
condition|)
name|get_tree
operator|=
literal|0
expr_stmt|;
block|}
if|if
condition|(
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
name|SCANNED
condition|)
return|return
literal|0
return|;
name|obj
operator|->
name|flags
operator||=
name|SCANNED
expr_stmt|;
if|if
condition|(
name|obj
operator|->
name|type
operator|==
name|commit_type
condition|)
block|{
if|if
condition|(
name|process_commit
argument_list|(
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
name|tree_type
condition|)
block|{
if|if
condition|(
name|process_tree
argument_list|(
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
name|blob_type
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
name|tag_type
condition|)
block|{
if|if
condition|(
name|process_tag
argument_list|(
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
name|obj
operator|->
name|type
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
name|object
modifier|*
name|obj
parameter_list|)
block|{
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
name|parse_object
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
expr_stmt|;
comment|/* We already have it, so we should scan it now. */
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
operator|(
name|SCANNED
operator||
name|TO_SCAN
operator|)
condition|)
return|return
literal|0
return|;
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
name|obj
operator|->
name|flags
operator||=
name|TO_SCAN
expr_stmt|;
return|return
literal|0
return|;
block|}
if|if
condition|(
name|obj
operator|->
name|flags
operator|&
operator|(
name|COMPLETE
operator||
name|TO_FETCH
operator|)
condition|)
return|return
literal|0
return|;
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
name|obj
operator|->
name|flags
operator||=
name|TO_FETCH
expr_stmt|;
name|prefetch
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
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
name|void
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
operator|!
name|has_sha1_file
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
operator|&&
name|fetch
argument_list|(
name|obj
operator|->
name|sha1
argument_list|)
condition|)
block|{
name|report_missing
argument_list|(
name|obj
operator|->
name|type
condition|?
name|obj
operator|->
name|type
else|:
literal|"object"
argument_list|,
name|obj
operator|->
name|sha1
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
name|check_ref_format
argument_list|(
name|target
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|fetch_ref
argument_list|(
name|target
argument_list|,
name|sha1
argument_list|)
condition|)
block|{
return|return
literal|0
return|;
block|}
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
DECL|function|pull
name|int
name|pull
parameter_list|(
name|char
modifier|*
name|target
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|int
name|fd
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
operator|&&
name|current_ref
condition|)
block|{
name|fd
operator|=
name|lock_ref_sha1
argument_list|(
name|write_ref
argument_list|,
name|current_ref
argument_list|)
expr_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
return|return
operator|-
literal|1
return|;
block|}
name|for_each_ref
argument_list|(
name|mark_complete
argument_list|)
expr_stmt|;
if|if
condition|(
name|interpret_target
argument_list|(
name|target
argument_list|,
name|sha1
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"Could not interpret %s as something to pull"
argument_list|,
name|target
argument_list|)
return|;
if|if
condition|(
name|process
argument_list|(
name|lookup_unknown_object
argument_list|(
name|sha1
argument_list|)
argument_list|)
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|loop
argument_list|()
condition|)
return|return
operator|-
literal|1
return|;
if|if
condition|(
name|write_ref
condition|)
block|{
if|if
condition|(
name|current_ref
condition|)
block|{
name|write_ref_sha1
argument_list|(
name|write_ref
argument_list|,
name|fd
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|write_ref_sha1_unlocked
argument_list|(
name|write_ref
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
block|}
block|}
return|return
literal|0
return|;
block|}
end_function
end_unit
