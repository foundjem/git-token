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
file|"refs.h"
end_include
begin_include
include|#
directive|include
file|"builtin.h"
end_include
begin_include
include|#
directive|include
file|"exec_cmd.h"
end_include
begin_include
include|#
directive|include
file|"parse-options.h"
end_include
begin_include
include|#
directive|include
file|"diff.h"
end_include
begin_include
include|#
directive|include
file|"hash.h"
end_include
begin_include
include|#
directive|include
file|"argv-array.h"
end_include
begin_define
DECL|macro|SEEN
define|#
directive|define
name|SEEN
value|(1u<<0)
end_define
begin_define
DECL|macro|MAX_TAGS
define|#
directive|define
name|MAX_TAGS
value|(FLAG_BITS - 1)
end_define
begin_decl_stmt
DECL|variable|describe_usage
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|describe_usage
index|[]
init|=
block|{
name|N_
argument_list|(
literal|"git describe [options]<commit-ish>*"
argument_list|)
block|,
name|N_
argument_list|(
literal|"git describe [options] --dirty"
argument_list|)
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|debug
specifier|static
name|int
name|debug
decl_stmt|;
end_decl_stmt
begin_comment
DECL|variable|debug
comment|/* Display lots of verbose info */
end_comment
begin_decl_stmt
DECL|variable|all
specifier|static
name|int
name|all
decl_stmt|;
end_decl_stmt
begin_comment
DECL|variable|all
comment|/* Any valid ref can be used */
end_comment
begin_decl_stmt
DECL|variable|tags
specifier|static
name|int
name|tags
decl_stmt|;
end_decl_stmt
begin_comment
DECL|variable|tags
comment|/* Allow lightweight tags */
end_comment
begin_decl_stmt
DECL|variable|longformat
specifier|static
name|int
name|longformat
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|first_parent
specifier|static
name|int
name|first_parent
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|abbrev
specifier|static
name|int
name|abbrev
init|=
operator|-
literal|1
decl_stmt|;
end_decl_stmt
begin_comment
DECL|variable|abbrev
comment|/* unspecified */
end_comment
begin_decl_stmt
DECL|variable|max_candidates
specifier|static
name|int
name|max_candidates
init|=
literal|10
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|names
specifier|static
name|struct
name|hash_table
name|names
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|have_util
specifier|static
name|int
name|have_util
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|pattern
specifier|static
specifier|const
name|char
modifier|*
name|pattern
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|always
specifier|static
name|int
name|always
decl_stmt|;
end_decl_stmt
begin_decl_stmt
DECL|variable|dirty
specifier|static
specifier|const
name|char
modifier|*
name|dirty
decl_stmt|;
end_decl_stmt
begin_comment
comment|/* diff-index command arguments to check if working tree is dirty. */
end_comment
begin_decl_stmt
DECL|variable|diff_index_args
specifier|static
specifier|const
name|char
modifier|*
name|diff_index_args
index|[]
init|=
block|{
literal|"diff-index"
block|,
literal|"--quiet"
block|,
literal|"HEAD"
block|,
literal|"--"
block|,
name|NULL
block|}
decl_stmt|;
end_decl_stmt
begin_struct
DECL|struct|commit_name
struct|struct
name|commit_name
block|{
DECL|member|next
name|struct
name|commit_name
modifier|*
name|next
decl_stmt|;
DECL|member|peeled
name|unsigned
name|char
name|peeled
index|[
literal|20
index|]
decl_stmt|;
DECL|member|tag
name|struct
name|tag
modifier|*
name|tag
decl_stmt|;
DECL|member|prio
name|unsigned
name|prio
range|:
literal|2
decl_stmt|;
comment|/* annotated tag = 2, tag = 1, head = 0 */
DECL|member|name_checked
name|unsigned
name|name_checked
range|:
literal|1
decl_stmt|;
DECL|member|sha1
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
DECL|member|path
name|char
modifier|*
name|path
decl_stmt|;
block|}
struct|;
end_struct
begin_decl_stmt
DECL|variable|prio_names
specifier|static
specifier|const
name|char
modifier|*
name|prio_names
index|[]
init|=
block|{
literal|"head"
block|,
literal|"lightweight"
block|,
literal|"annotated"
block|, }
decl_stmt|;
end_decl_stmt
begin_function
DECL|function|hash_sha1
specifier|static
specifier|inline
name|unsigned
name|int
name|hash_sha1
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|unsigned
name|int
name|hash
decl_stmt|;
name|memcpy
argument_list|(
operator|&
name|hash
argument_list|,
name|sha1
argument_list|,
sizeof|sizeof
argument_list|(
name|hash
argument_list|)
argument_list|)
expr_stmt|;
return|return
name|hash
return|;
block|}
end_function
begin_function
DECL|function|find_commit_name
specifier|static
specifier|inline
name|struct
name|commit_name
modifier|*
name|find_commit_name
parameter_list|(
specifier|const
name|unsigned
name|char
modifier|*
name|peeled
parameter_list|)
block|{
name|struct
name|commit_name
modifier|*
name|n
init|=
name|lookup_hash
argument_list|(
name|hash_sha1
argument_list|(
name|peeled
argument_list|)
argument_list|,
operator|&
name|names
argument_list|)
decl_stmt|;
while|while
condition|(
name|n
operator|&&
operator|!
operator|!
name|hashcmp
argument_list|(
name|peeled
argument_list|,
name|n
operator|->
name|peeled
argument_list|)
condition|)
name|n
operator|=
name|n
operator|->
name|next
expr_stmt|;
return|return
name|n
return|;
block|}
end_function
begin_function
DECL|function|set_util
specifier|static
name|int
name|set_util
parameter_list|(
name|void
modifier|*
name|chain
parameter_list|,
name|void
modifier|*
name|data
parameter_list|)
block|{
name|struct
name|commit_name
modifier|*
name|n
decl_stmt|;
for|for
control|(
name|n
operator|=
name|chain
init|;
name|n
condition|;
name|n
operator|=
name|n
operator|->
name|next
control|)
block|{
name|struct
name|commit
modifier|*
name|c
init|=
name|lookup_commit_reference_gently
argument_list|(
name|n
operator|->
name|peeled
argument_list|,
literal|1
argument_list|)
decl_stmt|;
if|if
condition|(
name|c
condition|)
name|c
operator|->
name|util
operator|=
name|n
expr_stmt|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|replace_name
specifier|static
name|int
name|replace_name
parameter_list|(
name|struct
name|commit_name
modifier|*
name|e
parameter_list|,
name|int
name|prio
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|,
name|struct
name|tag
modifier|*
modifier|*
name|tag
parameter_list|)
block|{
if|if
condition|(
operator|!
name|e
operator|||
name|e
operator|->
name|prio
operator|<
name|prio
condition|)
return|return
literal|1
return|;
if|if
condition|(
name|e
operator|->
name|prio
operator|==
literal|2
operator|&&
name|prio
operator|==
literal|2
condition|)
block|{
comment|/* Multiple annotated tags point to the same commit. 		 * Select one to keep based upon their tagger date. 		 */
name|struct
name|tag
modifier|*
name|t
decl_stmt|;
if|if
condition|(
operator|!
name|e
operator|->
name|tag
condition|)
block|{
name|t
operator|=
name|lookup_tag
argument_list|(
name|e
operator|->
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|t
operator|||
name|parse_tag
argument_list|(
name|t
argument_list|)
condition|)
return|return
literal|1
return|;
name|e
operator|->
name|tag
operator|=
name|t
expr_stmt|;
block|}
name|t
operator|=
name|lookup_tag
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|t
operator|||
name|parse_tag
argument_list|(
name|t
argument_list|)
condition|)
return|return
literal|0
return|;
operator|*
name|tag
operator|=
name|t
expr_stmt|;
if|if
condition|(
name|e
operator|->
name|tag
operator|->
name|date
operator|<
name|t
operator|->
name|date
condition|)
return|return
literal|1
return|;
block|}
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|add_to_known_names
specifier|static
name|void
name|add_to_known_names
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
name|peeled
parameter_list|,
name|int
name|prio
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|struct
name|commit_name
modifier|*
name|e
init|=
name|find_commit_name
argument_list|(
name|peeled
argument_list|)
decl_stmt|;
name|struct
name|tag
modifier|*
name|tag
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|replace_name
argument_list|(
name|e
argument_list|,
name|prio
argument_list|,
name|sha1
argument_list|,
operator|&
name|tag
argument_list|)
condition|)
block|{
if|if
condition|(
operator|!
name|e
condition|)
block|{
name|void
modifier|*
modifier|*
name|pos
decl_stmt|;
name|e
operator|=
name|xmalloc
argument_list|(
sizeof|sizeof
argument_list|(
expr|struct
name|commit_name
argument_list|)
argument_list|)
expr_stmt|;
name|hashcpy
argument_list|(
name|e
operator|->
name|peeled
argument_list|,
name|peeled
argument_list|)
expr_stmt|;
name|pos
operator|=
name|insert_hash
argument_list|(
name|hash_sha1
argument_list|(
name|peeled
argument_list|)
argument_list|,
name|e
argument_list|,
operator|&
name|names
argument_list|)
expr_stmt|;
if|if
condition|(
name|pos
condition|)
block|{
name|e
operator|->
name|next
operator|=
operator|*
name|pos
expr_stmt|;
operator|*
name|pos
operator|=
name|e
expr_stmt|;
block|}
else|else
block|{
name|e
operator|->
name|next
operator|=
name|NULL
expr_stmt|;
block|}
name|e
operator|->
name|path
operator|=
name|NULL
expr_stmt|;
block|}
name|e
operator|->
name|tag
operator|=
name|tag
expr_stmt|;
name|e
operator|->
name|prio
operator|=
name|prio
expr_stmt|;
name|e
operator|->
name|name_checked
operator|=
literal|0
expr_stmt|;
name|hashcpy
argument_list|(
name|e
operator|->
name|sha1
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|free
argument_list|(
name|e
operator|->
name|path
argument_list|)
expr_stmt|;
name|e
operator|->
name|path
operator|=
name|xstrdup
argument_list|(
name|path
argument_list|)
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|get_name
specifier|static
name|int
name|get_name
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
name|int
name|is_tag
init|=
operator|!
name|prefixcmp
argument_list|(
name|path
argument_list|,
literal|"refs/tags/"
argument_list|)
decl_stmt|;
name|unsigned
name|char
name|peeled
index|[
literal|20
index|]
decl_stmt|;
name|int
name|is_annotated
decl_stmt|,
name|prio
decl_stmt|;
comment|/* Reject anything outside refs/tags/ unless --all */
if|if
condition|(
operator|!
name|all
operator|&&
operator|!
name|is_tag
condition|)
return|return
literal|0
return|;
comment|/* Accept only tags that match the pattern, if given */
if|if
condition|(
name|pattern
operator|&&
operator|(
operator|!
name|is_tag
operator|||
name|fnmatch
argument_list|(
name|pattern
argument_list|,
name|path
operator|+
literal|10
argument_list|,
literal|0
argument_list|)
operator|)
condition|)
return|return
literal|0
return|;
comment|/* Is it annotated? */
if|if
condition|(
operator|!
name|peel_ref
argument_list|(
name|path
argument_list|,
name|peeled
argument_list|)
condition|)
block|{
name|is_annotated
operator|=
operator|!
operator|!
name|hashcmp
argument_list|(
name|sha1
argument_list|,
name|peeled
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|hashcpy
argument_list|(
name|peeled
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
name|is_annotated
operator|=
literal|0
expr_stmt|;
block|}
comment|/* 	 * By default, we only use annotated tags, but with --tags 	 * we fall back to lightweight ones (even without --tags, 	 * we still remember lightweight ones, only to give hints 	 * in an error message).  --all allows any refs to be used. 	 */
if|if
condition|(
name|is_annotated
condition|)
name|prio
operator|=
literal|2
expr_stmt|;
elseif|else
if|if
condition|(
name|is_tag
condition|)
name|prio
operator|=
literal|1
expr_stmt|;
else|else
name|prio
operator|=
literal|0
expr_stmt|;
name|add_to_known_names
argument_list|(
name|all
condition|?
name|path
operator|+
literal|5
else|:
name|path
operator|+
literal|10
argument_list|,
name|peeled
argument_list|,
name|prio
argument_list|,
name|sha1
argument_list|)
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_struct
DECL|struct|possible_tag
struct|struct
name|possible_tag
block|{
DECL|member|name
name|struct
name|commit_name
modifier|*
name|name
decl_stmt|;
DECL|member|depth
name|int
name|depth
decl_stmt|;
DECL|member|found_order
name|int
name|found_order
decl_stmt|;
DECL|member|flag_within
name|unsigned
name|flag_within
decl_stmt|;
block|}
struct|;
end_struct
begin_function
DECL|function|compare_pt
specifier|static
name|int
name|compare_pt
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
name|possible_tag
modifier|*
name|a
init|=
operator|(
expr|struct
name|possible_tag
operator|*
operator|)
name|a_
decl_stmt|;
name|struct
name|possible_tag
modifier|*
name|b
init|=
operator|(
expr|struct
name|possible_tag
operator|*
operator|)
name|b_
decl_stmt|;
if|if
condition|(
name|a
operator|->
name|depth
operator|!=
name|b
operator|->
name|depth
condition|)
return|return
name|a
operator|->
name|depth
operator|-
name|b
operator|->
name|depth
return|;
if|if
condition|(
name|a
operator|->
name|found_order
operator|!=
name|b
operator|->
name|found_order
condition|)
return|return
name|a
operator|->
name|found_order
operator|-
name|b
operator|->
name|found_order
return|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|finish_depth_computation
specifier|static
name|unsigned
name|long
name|finish_depth_computation
parameter_list|(
name|struct
name|commit_list
modifier|*
modifier|*
name|list
parameter_list|,
name|struct
name|possible_tag
modifier|*
name|best
parameter_list|)
block|{
name|unsigned
name|long
name|seen_commits
init|=
literal|0
decl_stmt|;
while|while
condition|(
operator|*
name|list
condition|)
block|{
name|struct
name|commit
modifier|*
name|c
init|=
name|pop_commit
argument_list|(
name|list
argument_list|)
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|c
operator|->
name|parents
decl_stmt|;
name|seen_commits
operator|++
expr_stmt|;
if|if
condition|(
name|c
operator|->
name|object
operator|.
name|flags
operator|&
name|best
operator|->
name|flag_within
condition|)
block|{
name|struct
name|commit_list
modifier|*
name|a
init|=
operator|*
name|list
decl_stmt|;
while|while
condition|(
name|a
condition|)
block|{
name|struct
name|commit
modifier|*
name|i
init|=
name|a
operator|->
name|item
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|i
operator|->
name|object
operator|.
name|flags
operator|&
name|best
operator|->
name|flag_within
operator|)
condition|)
break|break;
name|a
operator|=
name|a
operator|->
name|next
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|a
condition|)
break|break;
block|}
else|else
name|best
operator|->
name|depth
operator|++
expr_stmt|;
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|commit
modifier|*
name|p
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|parse_commit
argument_list|(
name|p
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|p
operator|->
name|object
operator|.
name|flags
operator|&
name|SEEN
operator|)
condition|)
name|commit_list_insert_by_date
argument_list|(
name|p
argument_list|,
name|list
argument_list|)
expr_stmt|;
name|p
operator|->
name|object
operator|.
name|flags
operator||=
name|c
operator|->
name|object
operator|.
name|flags
expr_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
block|}
block|}
return|return
name|seen_commits
return|;
block|}
end_function
begin_function
DECL|function|display_name
specifier|static
name|void
name|display_name
parameter_list|(
name|struct
name|commit_name
modifier|*
name|n
parameter_list|)
block|{
if|if
condition|(
name|n
operator|->
name|prio
operator|==
literal|2
operator|&&
operator|!
name|n
operator|->
name|tag
condition|)
block|{
name|n
operator|->
name|tag
operator|=
name|lookup_tag
argument_list|(
name|n
operator|->
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|n
operator|->
name|tag
operator|||
name|parse_tag
argument_list|(
name|n
operator|->
name|tag
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"annotated tag %s not available"
argument_list|)
argument_list|,
name|n
operator|->
name|path
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
name|n
operator|->
name|tag
operator|&&
operator|!
name|n
operator|->
name|name_checked
condition|)
block|{
if|if
condition|(
operator|!
name|n
operator|->
name|tag
operator|->
name|tag
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"annotated tag %s has no embedded name"
argument_list|)
argument_list|,
name|n
operator|->
name|path
argument_list|)
expr_stmt|;
if|if
condition|(
name|strcmp
argument_list|(
name|n
operator|->
name|tag
operator|->
name|tag
argument_list|,
name|all
condition|?
name|n
operator|->
name|path
operator|+
literal|5
else|:
name|n
operator|->
name|path
argument_list|)
condition|)
name|warning
argument_list|(
name|_
argument_list|(
literal|"tag '%s' is really '%s' here"
argument_list|)
argument_list|,
name|n
operator|->
name|tag
operator|->
name|tag
argument_list|,
name|n
operator|->
name|path
argument_list|)
expr_stmt|;
name|n
operator|->
name|name_checked
operator|=
literal|1
expr_stmt|;
block|}
if|if
condition|(
name|n
operator|->
name|tag
condition|)
name|printf
argument_list|(
literal|"%s"
argument_list|,
name|n
operator|->
name|tag
operator|->
name|tag
argument_list|)
expr_stmt|;
else|else
name|printf
argument_list|(
literal|"%s"
argument_list|,
name|n
operator|->
name|path
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|show_suffix
specifier|static
name|void
name|show_suffix
parameter_list|(
name|int
name|depth
parameter_list|,
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
parameter_list|)
block|{
name|printf
argument_list|(
literal|"-%d-g%s"
argument_list|,
name|depth
argument_list|,
name|find_unique_abbrev
argument_list|(
name|sha1
argument_list|,
name|abbrev
argument_list|)
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|describe
specifier|static
name|void
name|describe
parameter_list|(
specifier|const
name|char
modifier|*
name|arg
parameter_list|,
name|int
name|last_one
parameter_list|)
block|{
name|unsigned
name|char
name|sha1
index|[
literal|20
index|]
decl_stmt|;
name|struct
name|commit
modifier|*
name|cmit
decl_stmt|,
modifier|*
name|gave_up_on
init|=
name|NULL
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|list
decl_stmt|;
name|struct
name|commit_name
modifier|*
name|n
decl_stmt|;
name|struct
name|possible_tag
name|all_matches
index|[
name|MAX_TAGS
index|]
decl_stmt|;
name|unsigned
name|int
name|match_cnt
init|=
literal|0
decl_stmt|,
name|annotated_cnt
init|=
literal|0
decl_stmt|,
name|cur_match
decl_stmt|;
name|unsigned
name|long
name|seen_commits
init|=
literal|0
decl_stmt|;
name|unsigned
name|int
name|unannotated_cnt
init|=
literal|0
decl_stmt|;
if|if
condition|(
name|get_sha1
argument_list|(
name|arg
argument_list|,
name|sha1
argument_list|)
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"Not a valid object name %s"
argument_list|)
argument_list|,
name|arg
argument_list|)
expr_stmt|;
name|cmit
operator|=
name|lookup_commit_reference
argument_list|(
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|cmit
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"%s is not a valid '%s' object"
argument_list|)
argument_list|,
name|arg
argument_list|,
name|commit_type
argument_list|)
expr_stmt|;
name|n
operator|=
name|find_commit_name
argument_list|(
name|cmit
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|n
operator|&&
operator|(
name|tags
operator|||
name|all
operator|||
name|n
operator|->
name|prio
operator|==
literal|2
operator|)
condition|)
block|{
comment|/* 		 * Exact match to an existing ref. 		 */
name|display_name
argument_list|(
name|n
argument_list|)
expr_stmt|;
if|if
condition|(
name|longformat
condition|)
name|show_suffix
argument_list|(
literal|0
argument_list|,
name|n
operator|->
name|tag
condition|?
name|n
operator|->
name|tag
operator|->
name|tagged
operator|->
name|sha1
else|:
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|dirty
condition|)
name|printf
argument_list|(
literal|"%s"
argument_list|,
name|dirty
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
operator|!
name|max_candidates
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"no tag exactly matches '%s'"
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|cmit
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|debug
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"searching to describe %s\n"
argument_list|)
argument_list|,
name|arg
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|have_util
condition|)
block|{
name|for_each_hash
argument_list|(
operator|&
name|names
argument_list|,
name|set_util
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|have_util
operator|=
literal|1
expr_stmt|;
block|}
name|list
operator|=
name|NULL
expr_stmt|;
name|cmit
operator|->
name|object
operator|.
name|flags
operator|=
name|SEEN
expr_stmt|;
name|commit_list_insert
argument_list|(
name|cmit
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
while|while
condition|(
name|list
condition|)
block|{
name|struct
name|commit
modifier|*
name|c
init|=
name|pop_commit
argument_list|(
operator|&
name|list
argument_list|)
decl_stmt|;
name|struct
name|commit_list
modifier|*
name|parents
init|=
name|c
operator|->
name|parents
decl_stmt|;
name|seen_commits
operator|++
expr_stmt|;
name|n
operator|=
name|c
operator|->
name|util
expr_stmt|;
if|if
condition|(
name|n
condition|)
block|{
if|if
condition|(
operator|!
name|tags
operator|&&
operator|!
name|all
operator|&&
name|n
operator|->
name|prio
operator|<
literal|2
condition|)
block|{
name|unannotated_cnt
operator|++
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|match_cnt
operator|<
name|max_candidates
condition|)
block|{
name|struct
name|possible_tag
modifier|*
name|t
init|=
operator|&
name|all_matches
index|[
name|match_cnt
operator|++
index|]
decl_stmt|;
name|t
operator|->
name|name
operator|=
name|n
expr_stmt|;
name|t
operator|->
name|depth
operator|=
name|seen_commits
operator|-
literal|1
expr_stmt|;
name|t
operator|->
name|flag_within
operator|=
literal|1u
operator|<<
name|match_cnt
expr_stmt|;
name|t
operator|->
name|found_order
operator|=
name|match_cnt
expr_stmt|;
name|c
operator|->
name|object
operator|.
name|flags
operator||=
name|t
operator|->
name|flag_within
expr_stmt|;
if|if
condition|(
name|n
operator|->
name|prio
operator|==
literal|2
condition|)
name|annotated_cnt
operator|++
expr_stmt|;
block|}
else|else
block|{
name|gave_up_on
operator|=
name|c
expr_stmt|;
break|break;
block|}
block|}
for|for
control|(
name|cur_match
operator|=
literal|0
init|;
name|cur_match
operator|<
name|match_cnt
condition|;
name|cur_match
operator|++
control|)
block|{
name|struct
name|possible_tag
modifier|*
name|t
init|=
operator|&
name|all_matches
index|[
name|cur_match
index|]
decl_stmt|;
if|if
condition|(
operator|!
operator|(
name|c
operator|->
name|object
operator|.
name|flags
operator|&
name|t
operator|->
name|flag_within
operator|)
condition|)
name|t
operator|->
name|depth
operator|++
expr_stmt|;
block|}
if|if
condition|(
name|annotated_cnt
operator|&&
operator|!
name|list
condition|)
block|{
if|if
condition|(
name|debug
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"finished search at %s\n"
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|c
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
break|break;
block|}
while|while
condition|(
name|parents
condition|)
block|{
name|struct
name|commit
modifier|*
name|p
init|=
name|parents
operator|->
name|item
decl_stmt|;
name|parse_commit
argument_list|(
name|p
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
operator|(
name|p
operator|->
name|object
operator|.
name|flags
operator|&
name|SEEN
operator|)
condition|)
name|commit_list_insert_by_date
argument_list|(
name|p
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
name|p
operator|->
name|object
operator|.
name|flags
operator||=
name|c
operator|->
name|object
operator|.
name|flags
expr_stmt|;
name|parents
operator|=
name|parents
operator|->
name|next
expr_stmt|;
if|if
condition|(
name|first_parent
condition|)
break|break;
block|}
block|}
if|if
condition|(
operator|!
name|match_cnt
condition|)
block|{
specifier|const
name|unsigned
name|char
modifier|*
name|sha1
init|=
name|cmit
operator|->
name|object
operator|.
name|sha1
decl_stmt|;
if|if
condition|(
name|always
condition|)
block|{
name|printf
argument_list|(
literal|"%s"
argument_list|,
name|find_unique_abbrev
argument_list|(
name|sha1
argument_list|,
name|abbrev
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|dirty
condition|)
name|printf
argument_list|(
literal|"%s"
argument_list|,
name|dirty
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
return|return;
block|}
if|if
condition|(
name|unannotated_cnt
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"No annotated tags can describe '%s'.\n"
literal|"However, there were unannotated tags: try --tags."
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
else|else
name|die
argument_list|(
name|_
argument_list|(
literal|"No tags can describe '%s'.\n"
literal|"Try --always, or create some tags."
argument_list|)
argument_list|,
name|sha1_to_hex
argument_list|(
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
name|qsort
argument_list|(
name|all_matches
argument_list|,
name|match_cnt
argument_list|,
sizeof|sizeof
argument_list|(
name|all_matches
index|[
literal|0
index|]
argument_list|)
argument_list|,
name|compare_pt
argument_list|)
expr_stmt|;
if|if
condition|(
name|gave_up_on
condition|)
block|{
name|commit_list_insert_by_date
argument_list|(
name|gave_up_on
argument_list|,
operator|&
name|list
argument_list|)
expr_stmt|;
name|seen_commits
operator|--
expr_stmt|;
block|}
name|seen_commits
operator|+=
name|finish_depth_computation
argument_list|(
operator|&
name|list
argument_list|,
operator|&
name|all_matches
index|[
literal|0
index|]
argument_list|)
expr_stmt|;
name|free_commit_list
argument_list|(
name|list
argument_list|)
expr_stmt|;
if|if
condition|(
name|debug
condition|)
block|{
for|for
control|(
name|cur_match
operator|=
literal|0
init|;
name|cur_match
operator|<
name|match_cnt
condition|;
name|cur_match
operator|++
control|)
block|{
name|struct
name|possible_tag
modifier|*
name|t
init|=
operator|&
name|all_matches
index|[
name|cur_match
index|]
decl_stmt|;
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|" %-11s %8d %s\n"
argument_list|,
name|prio_names
index|[
name|t
operator|->
name|name
operator|->
name|prio
index|]
argument_list|,
name|t
operator|->
name|depth
argument_list|,
name|t
operator|->
name|name
operator|->
name|path
argument_list|)
expr_stmt|;
block|}
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"traversed %lu commits\n"
argument_list|)
argument_list|,
name|seen_commits
argument_list|)
expr_stmt|;
if|if
condition|(
name|gave_up_on
condition|)
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
name|_
argument_list|(
literal|"more than %i tags found; listed %i most recent\n"
literal|"gave up search at %s\n"
argument_list|)
argument_list|,
name|max_candidates
argument_list|,
name|max_candidates
argument_list|,
name|sha1_to_hex
argument_list|(
name|gave_up_on
operator|->
name|object
operator|.
name|sha1
argument_list|)
argument_list|)
expr_stmt|;
block|}
block|}
name|display_name
argument_list|(
name|all_matches
index|[
literal|0
index|]
operator|.
name|name
argument_list|)
expr_stmt|;
if|if
condition|(
name|abbrev
condition|)
name|show_suffix
argument_list|(
name|all_matches
index|[
literal|0
index|]
operator|.
name|depth
argument_list|,
name|cmit
operator|->
name|object
operator|.
name|sha1
argument_list|)
expr_stmt|;
if|if
condition|(
name|dirty
condition|)
name|printf
argument_list|(
literal|"%s"
argument_list|,
name|dirty
argument_list|)
expr_stmt|;
name|printf
argument_list|(
literal|"\n"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|last_one
condition|)
name|clear_commit_marks
argument_list|(
name|cmit
argument_list|,
operator|-
literal|1
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|cmd_describe
name|int
name|cmd_describe
parameter_list|(
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
name|contains
init|=
literal|0
decl_stmt|;
name|struct
name|option
name|options
index|[]
init|=
block|{
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"contains"
argument_list|,
operator|&
name|contains
argument_list|,
name|N_
argument_list|(
literal|"find the tag that comes after the commit"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"debug"
argument_list|,
operator|&
name|debug
argument_list|,
name|N_
argument_list|(
literal|"debug search strategy on stderr"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"all"
argument_list|,
operator|&
name|all
argument_list|,
name|N_
argument_list|(
literal|"use any ref"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"tags"
argument_list|,
operator|&
name|tags
argument_list|,
name|N_
argument_list|(
literal|"use any tag, even unannotated"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"long"
argument_list|,
operator|&
name|longformat
argument_list|,
name|N_
argument_list|(
literal|"always use long format"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"first-parent"
argument_list|,
operator|&
name|first_parent
argument_list|,
name|N_
argument_list|(
literal|"only follow first parent"
argument_list|)
argument_list|)
block|,
name|OPT__ABBREV
argument_list|(
operator|&
name|abbrev
argument_list|)
block|,
name|OPT_SET_INT
argument_list|(
literal|0
argument_list|,
literal|"exact-match"
argument_list|,
operator|&
name|max_candidates
argument_list|,
name|N_
argument_list|(
literal|"only output exact matches"
argument_list|)
argument_list|,
literal|0
argument_list|)
block|,
name|OPT_INTEGER
argument_list|(
literal|0
argument_list|,
literal|"candidates"
argument_list|,
operator|&
name|max_candidates
argument_list|,
name|N_
argument_list|(
literal|"consider<n> most recent tags (default: 10)"
argument_list|)
argument_list|)
block|,
name|OPT_STRING
argument_list|(
literal|0
argument_list|,
literal|"match"
argument_list|,
operator|&
name|pattern
argument_list|,
name|N_
argument_list|(
literal|"pattern"
argument_list|)
argument_list|,
name|N_
argument_list|(
literal|"only consider tags matching<pattern>"
argument_list|)
argument_list|)
block|,
name|OPT_BOOL
argument_list|(
literal|0
argument_list|,
literal|"always"
argument_list|,
operator|&
name|always
argument_list|,
name|N_
argument_list|(
literal|"show abbreviated commit object as fallback"
argument_list|)
argument_list|)
block|,
block|{
name|OPTION_STRING
block|,
literal|0
block|,
literal|"dirty"
block|,
operator|&
name|dirty
block|,
name|N_
argument_list|(
literal|"mark"
argument_list|)
block|,
name|N_
argument_list|(
literal|"append<mark> on dirty working tree (default: \"-dirty\")"
argument_list|)
block|,
name|PARSE_OPT_OPTARG
block|,
name|NULL
block|,
operator|(
name|intptr_t
operator|)
literal|"-dirty"
block|}
block|,
name|OPT_END
argument_list|()
block|, 	}
decl_stmt|;
name|git_config
argument_list|(
name|git_default_config
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|argc
operator|=
name|parse_options
argument_list|(
name|argc
argument_list|,
name|argv
argument_list|,
name|prefix
argument_list|,
name|options
argument_list|,
name|describe_usage
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
name|abbrev
operator|<
literal|0
condition|)
name|abbrev
operator|=
name|DEFAULT_ABBREV
expr_stmt|;
if|if
condition|(
name|max_candidates
operator|<
literal|0
condition|)
name|max_candidates
operator|=
literal|0
expr_stmt|;
elseif|else
if|if
condition|(
name|max_candidates
operator|>
name|MAX_TAGS
condition|)
name|max_candidates
operator|=
name|MAX_TAGS
expr_stmt|;
name|save_commit_buffer
operator|=
literal|0
expr_stmt|;
if|if
condition|(
name|longformat
operator|&&
name|abbrev
operator|==
literal|0
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"--long is incompatible with --abbrev=0"
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|contains
condition|)
block|{
name|struct
name|argv_array
name|args
decl_stmt|;
name|argv_array_init
argument_list|(
operator|&
name|args
argument_list|)
expr_stmt|;
name|argv_array_pushl
argument_list|(
operator|&
name|args
argument_list|,
literal|"name-rev"
argument_list|,
literal|"--peel-tag"
argument_list|,
literal|"--name-only"
argument_list|,
literal|"--no-undefined"
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|always
condition|)
name|argv_array_push
argument_list|(
operator|&
name|args
argument_list|,
literal|"--always"
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|all
condition|)
block|{
name|argv_array_push
argument_list|(
operator|&
name|args
argument_list|,
literal|"--tags"
argument_list|)
expr_stmt|;
if|if
condition|(
name|pattern
condition|)
name|argv_array_pushf
argument_list|(
operator|&
name|args
argument_list|,
literal|"--refs=refs/tags/%s"
argument_list|,
name|pattern
argument_list|)
expr_stmt|;
block|}
while|while
condition|(
operator|*
name|argv
condition|)
block|{
name|argv_array_push
argument_list|(
operator|&
name|args
argument_list|,
operator|*
name|argv
argument_list|)
expr_stmt|;
name|argv
operator|++
expr_stmt|;
block|}
return|return
name|cmd_name_rev
argument_list|(
name|args
operator|.
name|argc
argument_list|,
name|args
operator|.
name|argv
argument_list|,
name|prefix
argument_list|)
return|;
block|}
name|init_hash
argument_list|(
operator|&
name|names
argument_list|)
expr_stmt|;
name|for_each_rawref
argument_list|(
name|get_name
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|names
operator|.
name|nr
operator|&&
operator|!
name|always
condition|)
name|die
argument_list|(
name|_
argument_list|(
literal|"No names found, cannot describe anything."
argument_list|)
argument_list|)
expr_stmt|;
if|if
condition|(
name|argc
operator|==
literal|0
condition|)
block|{
if|if
condition|(
name|dirty
condition|)
block|{
specifier|static
name|struct
name|lock_file
name|index_lock
decl_stmt|;
name|int
name|fd
decl_stmt|;
name|read_cache_preload
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
name|refresh_index
argument_list|(
operator|&
name|the_index
argument_list|,
name|REFRESH_QUIET
operator||
name|REFRESH_UNMERGED
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|,
name|NULL
argument_list|)
expr_stmt|;
name|fd
operator|=
name|hold_locked_index
argument_list|(
operator|&
name|index_lock
argument_list|,
literal|0
argument_list|)
expr_stmt|;
if|if
condition|(
literal|0
operator|<=
name|fd
condition|)
name|update_index_if_able
argument_list|(
operator|&
name|the_index
argument_list|,
operator|&
name|index_lock
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|cmd_diff_index
argument_list|(
name|ARRAY_SIZE
argument_list|(
name|diff_index_args
argument_list|)
operator|-
literal|1
argument_list|,
name|diff_index_args
argument_list|,
name|prefix
argument_list|)
condition|)
name|dirty
operator|=
name|NULL
expr_stmt|;
block|}
name|describe
argument_list|(
literal|"HEAD"
argument_list|,
literal|1
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|dirty
condition|)
block|{
name|die
argument_list|(
name|_
argument_list|(
literal|"--dirty is incompatible with commit-ishes"
argument_list|)
argument_list|)
expr_stmt|;
block|}
else|else
block|{
while|while
condition|(
name|argc
operator|--
operator|>
literal|0
condition|)
block|{
name|describe
argument_list|(
operator|*
name|argv
operator|++
argument_list|,
name|argc
operator|==
literal|0
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
