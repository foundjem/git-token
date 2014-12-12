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
file|"progress.h"
end_include
begin_include
include|#
directive|include
file|"pack-revindex.h"
end_include
begin_include
include|#
directive|include
file|"pack.h"
end_include
begin_include
include|#
directive|include
file|"pack-bitmap.h"
end_include
begin_include
include|#
directive|include
file|"sha1-lookup.h"
end_include
begin_include
include|#
directive|include
file|"pack-objects.h"
end_include
begin_struct
DECL|struct|bitmapped_commit
struct|struct
name|bitmapped_commit
block|{
DECL|member|commit
name|struct
name|commit
modifier|*
name|commit
decl_stmt|;
DECL|member|bitmap
name|struct
name|ewah_bitmap
modifier|*
name|bitmap
decl_stmt|;
DECL|member|write_as
name|struct
name|ewah_bitmap
modifier|*
name|write_as
decl_stmt|;
DECL|member|flags
name|int
name|flags
decl_stmt|;
DECL|member|xor_offset
name|int
name|xor_offset
decl_stmt|;
DECL|member|commit_pos
name|uint32_t
name|commit_pos
decl_stmt|;
block|}
struct|;
end_struct
begin_struct
DECL|struct|bitmap_writer
struct|struct
name|bitmap_writer
block|{
DECL|member|commits
name|struct
name|ewah_bitmap
modifier|*
name|commits
decl_stmt|;
DECL|member|trees
name|struct
name|ewah_bitmap
modifier|*
name|trees
decl_stmt|;
DECL|member|blobs
name|struct
name|ewah_bitmap
modifier|*
name|blobs
decl_stmt|;
DECL|member|tags
name|struct
name|ewah_bitmap
modifier|*
name|tags
decl_stmt|;
DECL|member|bitmaps
name|khash_sha1
modifier|*
name|bitmaps
decl_stmt|;
DECL|member|reused
name|khash_sha1
modifier|*
name|reused
decl_stmt|;
DECL|member|to_pack
name|struct
name|packing_data
modifier|*
name|to_pack
decl_stmt|;
DECL|member|selected
name|struct
name|bitmapped_commit
modifier|*
name|selected
decl_stmt|;
DECL|member|selected_nr
DECL|member|selected_alloc
name|unsigned
name|int
name|selected_nr
decl_stmt|,
name|selected_alloc
decl_stmt|;
DECL|member|progress
name|struct
name|progress
modifier|*
name|progress
decl_stmt|;
DECL|member|show_progress
name|int
name|show_progress
decl_stmt|;
DECL|member|pack_checksum
name|unsigned
name|char
name|pack_checksum
index|[
literal|20
index|]
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|writer
specifier|static
name|struct
name|bitmap_writer
name|writer
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|bitmap_writer_show_progress
name|void
name|bitmap_writer_show_progress
parameter_list|(
name|int
name|show
parameter_list|)
block|{
name|writer
operator|.
name|show_progress
operator|=
name|show
expr_stmt|;
block|}
end_function
begin_comment
comment|/**  * Build the initial type index for the packfile  */
end_comment
begin_function
DECL|function|bitmap_writer_build_type_index
name|void
name|bitmap_writer_build_type_index
parameter_list|(
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|index
parameter_list|,
name|uint32_t
name|index_nr
parameter_list|)
block|{
name|uint32_t
name|i
decl_stmt|;
name|writer
operator|.
name|commits
operator|=
name|ewah_new
argument_list|()
expr_stmt|;
name|writer
operator|.
name|trees
operator|=
name|ewah_new
argument_list|()
expr_stmt|;
name|writer
operator|.
name|blobs
operator|=
name|ewah_new
argument_list|()
expr_stmt|;
name|writer
operator|.
name|tags
operator|=
name|ewah_new
argument_list|()
expr_stmt|;
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|index_nr
condition|;
operator|++
name|i
control|)
block|{
name|struct
name|object_entry
modifier|*
name|entry
init|=
operator|(
expr|struct
name|object_entry
operator|*
operator|)
name|index
index|[
name|i
index|]
decl_stmt|;
name|enum
name|object_type
name|real_type
decl_stmt|;
name|entry
operator|->
name|in_pack_pos
operator|=
name|i
expr_stmt|;
switch|switch
condition|(
name|entry
operator|->
name|type
condition|)
block|{
case|case
name|OBJ_COMMIT
case|:
case|case
name|OBJ_TREE
case|:
case|case
name|OBJ_BLOB
case|:
case|case
name|OBJ_TAG
case|:
name|real_type
operator|=
name|entry
operator|->
name|type
expr_stmt|;
break|break;
default|default:
name|real_type
operator|=
name|sha1_object_info
argument_list|(
name|entry
operator|->
name|idx
operator|.
name|sha1
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
break|break;
block|}
switch|switch
condition|(
name|real_type
condition|)
block|{
case|case
name|OBJ_COMMIT
case|:
name|ewah_set
argument_list|(
name|writer
operator|.
name|commits
argument_list|,
name|i
argument_list|)
expr_stmt|;
break|break;
case|case
name|OBJ_TREE
case|:
name|ewah_set
argument_list|(
name|writer
operator|.
name|trees
argument_list|,
name|i
argument_list|)
expr_stmt|;
break|break;
case|case
name|OBJ_BLOB
case|:
name|ewah_set
argument_list|(
name|writer
operator|.
name|blobs
argument_list|,
name|i
argument_list|)
expr_stmt|;
break|break;
case|case
name|OBJ_TAG
case|:
name|ewah_set
argument_list|(
name|writer
operator|.
name|tags
argument_list|,
name|i
argument_list|)
expr_stmt|;
break|break;
default|default:
name|die
argument_list|(
literal|"Missing type information for %s (%d/%d)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|entry
operator|->
name|idx
operator|.
name|sha1
argument_list|)
argument_list|,
name|real_type
argument_list|,
name|entry
operator|->
name|type
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_comment
comment|/**  * Compute the actual bitmaps  */
end_comment
begin_decl_stmt
DECL|variable|seen_objects
specifier|static
name|struct
name|object
modifier|*
modifier|*
name|seen_objects
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|seen_objects_nr
DECL|variable|seen_objects_alloc
specifier|static
name|unsigned
name|int
name|seen_objects_nr
decl_stmt|,
name|seen_objects_alloc
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|push_bitmapped_commit
specifier|static
specifier|inline
name|void
name|push_bitmapped_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|struct
name|ewah_bitmap
modifier|*
name|reused
parameter_list|)
block|{
if|if
condition|(
name|writer
operator|.
name|selected_nr
operator|>=
name|writer
operator|.
name|selected_alloc
condition|)
block|{
name|writer
operator|.
name|selected_alloc
operator|=
operator|(
name|writer
operator|.
name|selected_alloc
operator|+
literal|32
operator|)
operator|*
literal|2
expr_stmt|;
name|REALLOC_ARRAY
argument_list|(
name|writer
operator|.
name|selected
argument_list|,
name|writer
operator|.
name|selected_alloc
argument_list|)
expr_stmt|;
block|}
name|writer
operator|.
name|selected
index|[
name|writer
operator|.
name|selected_nr
index|]
operator|.
name|commit
operator|=
name|commit
expr_stmt|;
name|writer
operator|.
name|selected
index|[
name|writer
operator|.
name|selected_nr
index|]
operator|.
name|bitmap
operator|=
name|reused
expr_stmt|;
name|writer
operator|.
name|selected
index|[
name|writer
operator|.
name|selected_nr
index|]
operator|.
name|flags
operator|=
literal|0
expr_stmt|;
name|writer
operator|.
name|selected_nr
operator|++
expr_stmt|;
block|}
end_function
begin_function
DECL|function|mark_as_seen
specifier|static
specifier|inline
name|void
name|mark_as_seen
parameter_list|(
name|struct
name|object
modifier|*
name|object
parameter_list|)
block|{
name|ALLOC_GROW
argument_list|(
name|seen_objects
argument_list|,
name|seen_objects_nr
operator|+
literal|1
argument_list|,
name|seen_objects_alloc
argument_list|)
expr_stmt|;
name|seen_objects
index|[
name|seen_objects_nr
operator|++
index|]
operator|=
name|object
expr_stmt|;
block|}
end_function
begin_function
DECL|function|reset_all_seen
specifier|static
specifier|inline
name|void
name|reset_all_seen
parameter_list|(
name|void
parameter_list|)
block|{
name|unsigned
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
name|seen_objects_nr
condition|;
operator|++
name|i
control|)
block|{
name|seen_objects
index|[
name|i
index|]
operator|->
name|flags
operator|&=
operator|~
operator|(
name|SEEN
operator||
name|ADDED
operator||
name|SHOWN
operator|)
expr_stmt|;
block|}
name|seen_objects_nr
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|find_object_pos
specifier|static
name|uint32_t
name|find_object_pos
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|object_entry
modifier|*
name|entry
init|=
name|packlist_find
argument_list|(
name|writer
operator|.
name|to_pack
argument_list|,
name|sha1
argument_list|,
name|NULL
argument_list|)
decl_stmt|;
if|if
condition|(
operator|!
name|entry
condition|)
block|{
name|die
argument_list|(
literal|"Failed to write bitmap index. Packfile doesn't have full closure "
literal|"(object %s is missing)"
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
return|return
name|entry
operator|->
name|in_pack_pos
return|;
block|}
end_function
begin_function
DECL|function|show_object
specifier|static
name|void
name|show_object
parameter_list|(
name|struct
name|object
modifier|*
name|object
parameter_list|,
specifier|const
name|struct
name|name_path
modifier|*
name|path
parameter_list|,
specifier|const
name|char
modifier|*
name|last
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|struct
name|bitmap
modifier|*
name|base
init|=
name|data
decl_stmt|;
name|bitmap_set
argument_list|(
name|base
argument_list|,
name|find_object_pos
argument_list|(
name|object
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|mark_as_seen
argument_list|(
name|object
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_commit
specifier|static
name|void
name|show_commit
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|mark_as_seen
argument_list|(
operator|(
expr|struct
name|object
operator|*
operator|)
name|commit
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
specifier|static
name|int
DECL|function|add_to_include_set
name|add_to_include_set
parameter_list|(
name|struct
name|bitmap
modifier|*
name|base
parameter_list|,
name|struct
name|commit
modifier|*
name|commit
parameter_list|)
block|{
name|khiter_t
name|hash_pos
decl_stmt|;
name|uint32_t
name|bitmap_pos
init|=
name|find_object_pos
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
name|bitmap_get
argument_list|(
name|base
argument_list|,
name|bitmap_pos
argument_list|)
condition|)
return|return
literal|0
return|;
name|hash_pos
operator|=
name|kh_get_sha1
argument_list|(
name|writer
operator|.
name|bitmaps
argument_list|,
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|hash_pos
operator|<
name|kh_end
argument_list|(
name|writer
operator|.
name|bitmaps
argument_list|)
condition|)
block|{
name|struct
name|bitmapped_commit
modifier|*
name|bc
init|=
name|kh_value
argument_list|(
name|writer
operator|.
name|bitmaps
argument_list|,
name|hash_pos
argument_list|)
decl_stmt|;
name|bitmap_or_ewah
argument_list|(
name|base
argument_list|,
name|bc
operator|->
name|bitmap
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
name|bitmap_set
argument_list|(
name|base
argument_list|,
name|bitmap_pos
argument_list|)
expr_stmt|;
return|return
literal|1
return|;
block|}
end_function
begin_function
specifier|static
name|int
DECL|function|should_include
name|should_include
parameter_list|(
name|struct
name|commit
modifier|*
name|commit
parameter_list|,
name|void
modifier|*
name|_data
parameter_list|)
block|{
name|struct
name|bitmap
modifier|*
name|base
init|=
name|_data
decl_stmt|;
if|if
condition|(
operator|!
name|add_to_include_set
argument_list|(
name|base
argument_list|,
name|commit
argument_list|)
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|parent
init|=
name|commit
operator|->
name|parents
decl_stmt|;
name|mark_as_seen
argument_list|(
operator|(
expr|struct
name|object
operator|*
operator|)
name|commit
argument_list|)
expr_stmt|;
while|while
condition|(
name|parent
condition|)
block|{
name|parent
operator|->
name|item
operator|->
name|object
operator|.
name|flags
operator||=
name|SEEN
expr_stmt|;
name|mark_as_seen
argument_list|(
operator|(
expr|struct
name|object
operator|*
operator|)
name|parent
operator|->
name|item
argument_list|)
expr_stmt|;
name|parent
operator|=
name|parent
operator|->
name|next
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
return|return
literal|1
return|;
block|}
end_function
begin_function
DECL|function|compute_xor_offsets
specifier|static
name|void
name|compute_xor_offsets
parameter_list|(
name|void
parameter_list|)
block|{
specifier|static
specifier|const
name|int
name|MAX_XOR_OFFSET_SEARCH
init|=
literal|10
decl_stmt|;
name|int
name|i
decl_stmt|,
name|next
init|=
literal|0
decl_stmt|;
while|while
condition|(
name|next
operator|<
name|writer
operator|.
name|selected_nr
condition|)
block|{
name|struct
name|bitmapped_commit
modifier|*
name|stored
init|=
operator|&
name|writer
operator|.
name|selected
index|[
name|next
index|]
decl_stmt|;
name|int
name|best_offset
init|=
literal|0
decl_stmt|;
name|struct
name|ewah_bitmap
modifier|*
name|best_bitmap
init|=
name|stored
operator|->
name|bitmap
decl_stmt|;
name|struct
name|ewah_bitmap
modifier|*
name|test_xor
decl_stmt|;
for|for
control|(
name|i
operator|=
literal|1
init|;
name|i
operator|<=
name|MAX_XOR_OFFSET_SEARCH
condition|;
operator|++
name|i
control|)
block|{
name|int
name|curr
init|=
name|next
operator|-
name|i
decl_stmt|;
if|if
condition|(
name|curr
operator|<
literal|0
condition|)
break|break;
name|test_xor
operator|=
name|ewah_pool_new
argument_list|()
expr_stmt|;
name|ewah_xor
argument_list|(
name|writer
operator|.
name|selected
index|[
name|curr
index|]
operator|.
name|bitmap
argument_list|,
name|stored
operator|->
name|bitmap
argument_list|,
name|test_xor
argument_list|)
expr_stmt|;
if|if
condition|(
name|test_xor
operator|->
name|buffer_size
operator|<
name|best_bitmap
operator|->
name|buffer_size
condition|)
block|{
if|if
condition|(
name|best_bitmap
operator|!=
name|stored
operator|->
name|bitmap
condition|)
name|ewah_pool_free
argument_list|(
name|best_bitmap
argument_list|)
expr_stmt|;
name|best_bitmap
operator|=
name|test_xor
expr_stmt|;
name|best_offset
operator|=
name|i
expr_stmt|;
block|}
else|else
block|{
name|ewah_pool_free
argument_list|(
name|test_xor
argument_list|)
expr_stmt|;
block|}
block|}
name|stored
operator|->
name|xor_offset
operator|=
name|best_offset
expr_stmt|;
name|stored
operator|->
name|write_as
operator|=
name|best_bitmap
expr_stmt|;
name|next
operator|++
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|bitmap_writer_build
name|void
name|bitmap_writer_build
parameter_list|(
name|struct
name|packing_data
modifier|*
name|to_pack
parameter_list|)
block|{
specifier|static
specifier|const
name|double
name|REUSE_BITMAP_THRESHOLD
init|=
literal|0.2
decl_stmt|;
name|int
name|i
decl_stmt|,
name|reuse_after
decl_stmt|,
name|need_reset
decl_stmt|;
name|struct
name|bitmap
modifier|*
name|base
init|=
name|bitmap_new
argument_list|()
decl_stmt|;
name|struct
name|rev_info
name|revs
decl_stmt|;
name|writer
operator|.
name|bitmaps
operator|=
name|kh_init_sha1
argument_list|()
expr_stmt|;
name|writer
operator|.
name|to_pack
operator|=
name|to_pack
expr_stmt|;
if|if
condition|(
name|writer
operator|.
name|show_progress
condition|)
name|writer
operator|.
name|progress
operator|=
name|start_progress
argument_list|(
literal|"Building bitmaps"
argument_list|,
name|writer
operator|.
name|selected_nr
argument_list|)
expr_stmt|;
name|init_revisions
argument_list|(
operator|&
name|revs
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|revs
operator|.
name|tag_objects
operator|=
literal|1
expr_stmt|;
name|revs
operator|.
name|tree_objects
operator|=
literal|1
expr_stmt|;
name|revs
operator|.
name|blob_objects
operator|=
literal|1
expr_stmt|;
name|revs
operator|.
name|no_walk
operator|=
literal|0
expr_stmt|;
name|revs
operator|.
name|include_check
operator|=
name|should_include
expr_stmt|;
name|reset_revision_walk
argument_list|()
expr_stmt|;
name|reuse_after
operator|=
name|writer
operator|.
name|selected_nr
operator|*
name|REUSE_BITMAP_THRESHOLD
expr_stmt|;
name|need_reset
operator|=
literal|0
expr_stmt|;
for|for
control|(
name|i
operator|=
name|writer
operator|.
name|selected_nr
operator|-
literal|1
init|;
name|i
operator|>=
literal|0
condition|;
operator|--
name|i
control|)
block|{
name|struct
name|bitmapped_commit
modifier|*
name|stored
decl_stmt|;
name|struct
name|object
modifier|*
name|object
decl_stmt|;
name|khiter_t
name|hash_pos
decl_stmt|;
name|int
name|hash_ret
decl_stmt|;
name|stored
operator|=
operator|&
name|writer
operator|.
name|selected
index|[
name|i
index|]
expr_stmt|;
name|object
operator|=
operator|(
expr|struct
name|object
operator|*
operator|)
name|stored
operator|->
name|commit
expr_stmt|;
if|if
condition|(
name|stored
operator|->
name|bitmap
operator|==
name|NULL
condition|)
block|{
if|if
condition|(
name|i
operator|<
name|writer
operator|.
name|selected_nr
operator|-
literal|1
operator|&&
operator|(
name|need_reset
operator|||
operator|!
name|in_merge_bases
argument_list|(
name|writer
operator|.
name|selected
index|[
name|i
operator|+
literal|1
index|]
operator|.
name|commit
argument_list|,
name|stored
operator|->
name|commit
argument_list|)
operator|)
condition|)
block|{
name|bitmap_reset
argument_list|(
name|base
argument_list|)
expr_stmt|;
name|reset_all_seen
argument_list|()
expr_stmt|;
block|}
name|add_pending_object
argument_list|(
operator|&
name|revs
argument_list|,
name|object
argument_list|,
literal|""
argument_list|)
expr_stmt|;
name|revs
operator|.
name|include_check_data
operator|=
name|base
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
literal|"revision walk setup failed"
argument_list|)
expr_stmt|;
name|traverse_commit_list
argument_list|(
operator|&
name|revs
argument_list|,
name|show_commit
argument_list|,
name|show_object
argument_list|,
name|base
argument_list|)
expr_stmt|;
name|revs
operator|.
name|pending
operator|.
name|nr
operator|=
literal|0
expr_stmt|;
name|revs
operator|.
name|pending
operator|.
name|alloc
operator|=
literal|0
expr_stmt|;
name|revs
operator|.
name|pending
operator|.
name|objects
operator|=
name|NULL
expr_stmt|;
name|stored
operator|->
name|bitmap
operator|=
name|bitmap_to_ewah
argument_list|(
name|base
argument_list|)
expr_stmt|;
name|need_reset
operator|=
literal|0
expr_stmt|;
block|}
else|else
name|need_reset
operator|=
literal|1
expr_stmt|;
if|if
condition|(
name|i
operator|>=
name|reuse_after
condition|)
name|stored
operator|->
name|flags
operator||=
name|BITMAP_FLAG_REUSE
expr_stmt|;
name|hash_pos
operator|=
name|kh_put_sha1
argument_list|(
name|writer
operator|.
name|bitmaps
argument_list|,
name|object
operator|->
name|sha1
argument_list|,
operator|&
name|hash_ret
argument_list|)
expr_stmt|;
if|if
condition|(
name|hash_ret
operator|==
literal|0
condition|)
name|die
argument_list|(
literal|"Duplicate entry when writing index: %s"
argument_list|,
name|sha1_to_hex
argument_list|(
name|object
operator|->
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
name|kh_value
argument_list|(
name|writer
operator|.
name|bitmaps
argument_list|,
name|hash_pos
argument_list|)
operator|=
name|stored
expr_stmt|;
name|display_progress
argument_list|(
name|writer
operator|.
name|progress
argument_list|,
name|writer
operator|.
name|selected_nr
operator|-
name|i
argument_list|)
expr_stmt|;
block|}
name|bitmap_free
argument_list|(
name|base
argument_list|)
expr_stmt|;
name|stop_progress
argument_list|(
operator|&
name|writer
operator|.
name|progress
argument_list|)
expr_stmt|;
name|compute_xor_offsets
argument_list|()
expr_stmt|;
block|}
end_function
begin_comment
comment|/**  * Select the commits that will be bitmapped  */
end_comment
begin_function
DECL|function|next_commit_index
specifier|static
specifier|inline
name|unsigned
name|int
name|next_commit_index
parameter_list|(
name|unsigned
name|int
name|idx
parameter_list|)
block|{
specifier|static
specifier|const
name|unsigned
name|int
name|MIN_COMMITS
init|=
literal|100
decl_stmt|;
specifier|static
specifier|const
name|unsigned
name|int
name|MAX_COMMITS
init|=
literal|5000
decl_stmt|;
specifier|static
specifier|const
name|unsigned
name|int
name|MUST_REGION
init|=
literal|100
decl_stmt|;
specifier|static
specifier|const
name|unsigned
name|int
name|MIN_REGION
init|=
literal|20000
decl_stmt|;
name|unsigned
name|int
name|offset
decl_stmt|,
name|next
decl_stmt|;
if|if
condition|(
name|idx
operator|<=
name|MUST_REGION
condition|)
return|return
literal|0
return|;
if|if
condition|(
name|idx
operator|<=
name|MIN_REGION
condition|)
block|{
name|offset
operator|=
name|idx
operator|-
name|MUST_REGION
expr_stmt|;
return|return
operator|(
name|offset
operator|<
name|MIN_COMMITS
operator|)
condition|?
name|offset
else|:
name|MIN_COMMITS
return|;
block|}
name|offset
operator|=
name|idx
operator|-
name|MIN_REGION
expr_stmt|;
name|next
operator|=
operator|(
name|offset
operator|<
name|MAX_COMMITS
operator|)
condition|?
name|offset
else|:
name|MAX_COMMITS
expr_stmt|;
return|return
operator|(
name|next
operator|>
name|MIN_COMMITS
operator|)
condition|?
name|next
else|:
name|MIN_COMMITS
return|;
block|}
end_function
begin_function
DECL|function|date_compare
specifier|static
name|int
name|date_compare
parameter_list|(
specifier|const
name|void
modifier|*
name|_a
parameter_list|,
specifier|const
name|void
modifier|*
name|_b
parameter_list|)
block|{
name|struct
name|commit
modifier|*
name|a
init|=
operator|*
operator|(
expr|struct
name|commit
operator|*
operator|*
operator|)
name|_a
decl_stmt|;
name|struct
name|commit
modifier|*
name|b
init|=
operator|*
operator|(
expr|struct
name|commit
operator|*
operator|*
operator|)
name|_b
decl_stmt|;
return|return
operator|(
name|long
operator|)
name|b
operator|->
name|date
operator|-
operator|(
name|long
operator|)
name|a
operator|->
name|date
return|;
block|}
end_function
begin_function
DECL|function|bitmap_writer_reuse_bitmaps
name|void
name|bitmap_writer_reuse_bitmaps
parameter_list|(
name|struct
name|packing_data
modifier|*
name|to_pack
parameter_list|)
block|{
if|if
condition|(
name|prepare_bitmap_git
argument_list|()
operator|<
literal|0
condition|)
return|return;
name|writer
operator|.
name|reused
operator|=
name|kh_init_sha1
argument_list|()
expr_stmt|;
name|rebuild_existing_bitmaps
argument_list|(
name|to_pack
argument_list|,
name|writer
operator|.
name|reused
argument_list|,
name|writer
operator|.
name|show_progress
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|find_reused_bitmap
specifier|static
name|struct
name|ewah_bitmap
modifier|*
name|find_reused_bitmap
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|khiter_t
name|hash_pos
decl_stmt|;
if|if
condition|(
operator|!
name|writer
operator|.
name|reused
condition|)
return|return
name|NULL
return|;
name|hash_pos
operator|=
name|kh_get_sha1
argument_list|(
name|writer
operator|.
name|reused
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|hash_pos
operator|>=
name|kh_end
argument_list|(
name|writer
operator|.
name|reused
argument_list|)
condition|)
return|return
name|NULL
return|;
return|return
name|kh_value
argument_list|(
name|writer
operator|.
name|reused
argument_list|,
name|hash_pos
argument_list|)
return|;
block|}
end_function
begin_function
DECL|function|bitmap_writer_select_commits
name|void
name|bitmap_writer_select_commits
parameter_list|(
name|struct
name|commit
modifier|*
modifier|*
name|indexed_commits
parameter_list|,
name|unsigned
name|int
name|indexed_commits_nr
parameter_list|,
name|int
name|max_bitmaps
parameter_list|)
block|{
name|unsigned
name|int
name|i
init|=
literal|0
decl_stmt|,
name|j
decl_stmt|,
name|next
decl_stmt|;
name|qsort
argument_list|(
name|indexed_commits
argument_list|,
name|indexed_commits_nr
argument_list|,
sizeof|sizeof
argument_list|(
name|indexed_commits
index|[
literal|0
index|]
argument_list|)
argument_list|,
name|date_compare
argument_list|)
expr_stmt|;
if|if
condition|(
name|writer
operator|.
name|show_progress
condition|)
name|writer
operator|.
name|progress
operator|=
name|start_progress
argument_list|(
literal|"Selecting bitmap commits"
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|indexed_commits_nr
operator|<
literal|100
condition|)
block|{
for|for
control|(
name|i
operator|=
literal|0
init|;
name|i
operator|<
name|indexed_commits_nr
condition|;
operator|++
name|i
control|)
name|push_bitmapped_commit
argument_list|(
name|indexed_commits
index|[
name|i
index|]
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
return|return;
block|}
for|for
control|(
init|;
condition|;
control|)
block|{
name|struct
name|ewah_bitmap
modifier|*
name|reused_bitmap
init|=
name|NULL
decl_stmt|;
name|struct
name|commit
modifier|*
name|chosen
init|=
name|NULL
decl_stmt|;
name|next
operator|=
name|next_commit_index
argument_list|(
name|i
argument_list|)
expr_stmt|;
if|if
condition|(
name|i
operator|+
name|next
operator|>=
name|indexed_commits_nr
condition|)
break|break;
if|if
condition|(
name|max_bitmaps
operator|>
literal|0
operator|&&
name|writer
operator|.
name|selected_nr
operator|>=
name|max_bitmaps
condition|)
block|{
name|writer
operator|.
name|selected_nr
operator|=
name|max_bitmaps
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|next
operator|==
literal|0
condition|)
block|{
name|chosen
operator|=
name|indexed_commits
index|[
name|i
index|]
expr_stmt|;
name|reused_bitmap
operator|=
name|find_reused_bitmap
argument_list|(
name|chosen
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|chosen
operator|=
name|indexed_commits
index|[
name|i
operator|+
name|next
index|]
expr_stmt|;
for|for
control|(
name|j
operator|=
literal|0
init|;
name|j
operator|<=
name|next
condition|;
operator|++
name|j
control|)
block|{
name|struct
name|commit
modifier|*
name|cm
init|=
name|indexed_commits
index|[
name|i
operator|+
name|j
index|]
decl_stmt|;
name|reused_bitmap
operator|=
name|find_reused_bitmap
argument_list|(
name|cm
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|reused_bitmap
operator|||
operator|(
name|cm
operator|->
name|object
operator|.
name|flags
operator|&
name|NEEDS_BITMAP
operator|)
operator|!=
literal|0
condition|)
block|{
name|chosen
operator|=
name|cm
expr_stmt|;
break|break;
block|}
if|if
condition|(
name|cm
operator|->
name|parents
operator|&&
name|cm
operator|->
name|parents
operator|->
name|next
condition|)
name|chosen
operator|=
name|cm
expr_stmt|;
block|}
block|}
name|push_bitmapped_commit
argument_list|(
name|chosen
argument_list|,
name|reused_bitmap
argument_list|)
expr_stmt|;
name|i
operator|+=
name|next
operator|+
literal|1
expr_stmt|;
name|display_progress
argument_list|(
name|writer
operator|.
name|progress
argument_list|,
name|i
argument_list|)
expr_stmt|;
block|}
name|stop_progress
argument_list|(
operator|&
name|writer
operator|.
name|progress
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|sha1write_ewah_helper
specifier|static
name|int
name|sha1write_ewah_helper
parameter_list|(
name|void
modifier|*
name|f
parameter_list|,
specifier|const
name|void
modifier|*
name|buf
parameter_list|,
name|size_t
name|len
parameter_list|)
block|{
comment|/* sha1write will die on error */
name|sha1write
argument_list|(
name|f
argument_list|,
name|buf
argument_list|,
name|len
argument_list|)
expr_stmt|;
return|return
name|len
return|;
block|}
end_function
begin_comment
comment|/**  * Write the bitmap index to disk  */
end_comment
begin_function
DECL|function|dump_bitmap
specifier|static
specifier|inline
name|void
name|dump_bitmap
parameter_list|(
name|struct
name|sha1file
modifier|*
name|f
parameter_list|,
name|struct
name|ewah_bitmap
modifier|*
name|bitmap
parameter_list|)
block|{
if|if
condition|(
name|ewah_serialize_to
argument_list|(
name|bitmap
argument_list|,
name|sha1write_ewah_helper
argument_list|,
name|f
argument_list|)
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"Failed to write bitmap index"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|sha1_access
specifier|static
specifier|const
name|unsigned
name|char
modifier|*
name|sha1_access
parameter_list|(
name|size_t
name|pos
parameter_list|,
name|void
modifier|*
name|table
parameter_list|)
block|{
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|index
init|=
name|table
decl_stmt|;
return|return
name|index
index|[
name|pos
index|]
operator|->
name|sha1
return|;
block|}
end_function
begin_function
DECL|function|write_selected_commits_v1
specifier|static
name|void
name|write_selected_commits_v1
parameter_list|(
name|struct
name|sha1file
modifier|*
name|f
parameter_list|,
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|index
parameter_list|,
name|uint32_t
name|index_nr
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
name|writer
operator|.
name|selected_nr
condition|;
operator|++
name|i
control|)
block|{
name|struct
name|bitmapped_commit
modifier|*
name|stored
init|=
operator|&
name|writer
operator|.
name|selected
index|[
name|i
index|]
decl_stmt|;
name|int
name|commit_pos
init|=
name|sha1_pos
argument_list|(
name|stored
operator|->
name|commit
operator|->
name|object
operator|.
name|sha1
argument_list|,
name|index
argument_list|,
name|index_nr
argument_list|,
name|sha1_access
argument_list|)
decl_stmt|;
if|if
condition|(
name|commit_pos
operator|<
literal|0
condition|)
name|die
argument_list|(
literal|"BUG: trying to write commit not in index"
argument_list|)
expr_stmt|;
name|sha1write_be32
argument_list|(
name|f
argument_list|,
name|commit_pos
argument_list|)
expr_stmt|;
name|sha1write_u8
argument_list|(
name|f
argument_list|,
name|stored
operator|->
name|xor_offset
argument_list|)
expr_stmt|;
name|sha1write_u8
argument_list|(
name|f
argument_list|,
name|stored
operator|->
name|flags
argument_list|)
expr_stmt|;
name|dump_bitmap
argument_list|(
name|f
argument_list|,
name|stored
operator|->
name|write_as
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|write_hash_cache
specifier|static
name|void
name|write_hash_cache
parameter_list|(
name|struct
name|sha1file
modifier|*
name|f
parameter_list|,
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|index
parameter_list|,
name|uint32_t
name|index_nr
parameter_list|)
block|{
name|uint32_t
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
name|index_nr
condition|;
operator|++
name|i
control|)
block|{
name|struct
name|object_entry
modifier|*
name|entry
init|=
operator|(
expr|struct
name|object_entry
operator|*
operator|)
name|index
index|[
name|i
index|]
decl_stmt|;
name|uint32_t
name|hash_value
init|=
name|htonl
argument_list|(
name|entry
operator|->
name|hash
argument_list|)
decl_stmt|;
name|sha1write
argument_list|(
name|f
argument_list|,
operator|&
name|hash_value
argument_list|,
sizeof|sizeof
argument_list|(
name|hash_value
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|bitmap_writer_set_checksum
name|void
name|bitmap_writer_set_checksum
parameter_list|(
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|hashcpy
argument_list|(
name|writer
operator|.
name|pack_checksum
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|bitmap_writer_finish
name|void
name|bitmap_writer_finish
parameter_list|(
name|struct
name|pack_idx_entry
modifier|*
modifier|*
name|index
parameter_list|,
name|uint32_t
name|index_nr
parameter_list|,
specifier|const
name|char
modifier|*
name|filename
parameter_list|,
name|uint16_t
name|options
parameter_list|)
block|{
specifier|static
name|char
name|tmp_file
index|[
name|PATH_MAX
index|]
decl_stmt|;
specifier|static
name|uint16_t
name|default_version
init|=
literal|1
decl_stmt|;
specifier|static
name|uint16_t
name|flags
init|=
name|BITMAP_OPT_FULL_DAG
decl_stmt|;
name|struct
name|sha1file
modifier|*
name|f
decl_stmt|;
name|struct
name|bitmap_disk_header
name|header
decl_stmt|;
name|int
name|fd
init|=
name|odb_mkstemp
argument_list|(
name|tmp_file
argument_list|,
sizeof|sizeof
argument_list|(
name|tmp_file
argument_list|)
argument_list|,
literal|"pack/tmp_bitmap_XXXXXX"
argument_list|)
decl_stmt|;
if|if
condition|(
name|fd
operator|<
literal|0
condition|)
name|die_errno
argument_list|(
literal|"unable to create '%s'"
argument_list|,
name|tmp_file
argument_list|)
expr_stmt|;
name|f
operator|=
name|sha1fd
argument_list|(
name|fd
argument_list|,
name|tmp_file
argument_list|)
expr_stmt|;
name|memcpy
argument_list|(
name|header
operator|.
name|magic
argument_list|,
name|BITMAP_IDX_SIGNATURE
argument_list|,
sizeof|sizeof
argument_list|(
name|BITMAP_IDX_SIGNATURE
argument_list|)
argument_list|)
expr_stmt|;
name|header
operator|.
name|version
operator|=
name|htons
argument_list|(
name|default_version
argument_list|)
expr_stmt|;
name|header
operator|.
name|options
operator|=
name|htons
argument_list|(
name|flags
operator||
name|options
argument_list|)
expr_stmt|;
name|header
operator|.
name|entry_count
operator|=
name|htonl
argument_list|(
name|writer
operator|.
name|selected_nr
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|header
operator|.
name|checksum
argument_list|,
name|writer
operator|.
name|pack_checksum
argument_list|)
expr_stmt|;
name|sha1write
argument_list|(
name|f
argument_list|,
operator|&
name|header
argument_list|,
sizeof|sizeof
argument_list|(
name|header
argument_list|)
argument_list|)
expr_stmt|;
name|dump_bitmap
argument_list|(
name|f
argument_list|,
name|writer
operator|.
name|commits
argument_list|)
expr_stmt|;
name|dump_bitmap
argument_list|(
name|f
argument_list|,
name|writer
operator|.
name|trees
argument_list|)
expr_stmt|;
name|dump_bitmap
argument_list|(
name|f
argument_list|,
name|writer
operator|.
name|blobs
argument_list|)
expr_stmt|;
name|dump_bitmap
argument_list|(
name|f
argument_list|,
name|writer
operator|.
name|tags
argument_list|)
expr_stmt|;
name|write_selected_commits_v1
argument_list|(
name|f
argument_list|,
name|index
argument_list|,
name|index_nr
argument_list|)
expr_stmt|;
if|if
condition|(
name|options
operator|&
name|BITMAP_OPT_HASH_CACHE
condition|)
name|write_hash_cache
argument_list|(
name|f
argument_list|,
name|index
argument_list|,
name|index_nr
argument_list|)
expr_stmt|;
name|sha1close
argument_list|(
name|f
argument_list|,
name|NULL
argument_list|,
name|CSUM_FSYNC
argument_list|)
expr_stmt|;
if|if
condition|(
name|adjust_shared_perm
argument_list|(
name|tmp_file
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"unable to make temporary bitmap file readable"
argument_list|)
expr_stmt|;
if|if
condition|(
name|rename
argument_list|(
name|tmp_file
argument_list|,
name|filename
argument_list|)
condition|)
name|die_errno
argument_list|(
literal|"unable to rename temporary bitmap file to '%s'"
argument_list|,
name|filename
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
