begin_unit
begin_comment
comment|/*  * Parse and rearrange a svnadmin dump.  * Create the dump with:  * svnadmin dump --incremental -r<startrev>:<endrev><repository>>outfile  *  * Licensed under a two-clause BSD-style license.  * See LICENSE for details.  */
end_comment
begin_include
include|#
directive|include
file|"cache.h"
end_include
begin_include
include|#
directive|include
file|"repo_tree.h"
end_include
begin_include
include|#
directive|include
file|"fast_export.h"
end_include
begin_include
include|#
directive|include
file|"line_buffer.h"
end_include
begin_include
include|#
directive|include
file|"obj_pool.h"
end_include
begin_include
include|#
directive|include
file|"string_pool.h"
end_include
begin_define
DECL|macro|REPORT_FILENO
define|#
directive|define
name|REPORT_FILENO
value|3
end_define
begin_define
DECL|macro|NODEACT_REPLACE
define|#
directive|define
name|NODEACT_REPLACE
value|4
end_define
begin_define
DECL|macro|NODEACT_DELETE
define|#
directive|define
name|NODEACT_DELETE
value|3
end_define
begin_define
DECL|macro|NODEACT_ADD
define|#
directive|define
name|NODEACT_ADD
value|2
end_define
begin_define
DECL|macro|NODEACT_CHANGE
define|#
directive|define
name|NODEACT_CHANGE
value|1
end_define
begin_define
DECL|macro|NODEACT_UNKNOWN
define|#
directive|define
name|NODEACT_UNKNOWN
value|0
end_define
begin_comment
comment|/* States: */
end_comment
begin_define
DECL|macro|DUMP_CTX
define|#
directive|define
name|DUMP_CTX
value|0
end_define
begin_comment
DECL|macro|DUMP_CTX
comment|/* dump metadata */
end_comment
begin_define
DECL|macro|REV_CTX
define|#
directive|define
name|REV_CTX
value|1
end_define
begin_comment
DECL|macro|REV_CTX
comment|/* revision metadata */
end_comment
begin_define
DECL|macro|NODE_CTX
define|#
directive|define
name|NODE_CTX
value|2
end_define
begin_comment
DECL|macro|NODE_CTX
comment|/* node metadata */
end_comment
begin_define
DECL|macro|INTERNODE_CTX
define|#
directive|define
name|INTERNODE_CTX
value|3
end_define
begin_comment
DECL|macro|INTERNODE_CTX
comment|/* between nodes */
end_comment
begin_define
DECL|macro|LENGTH_UNKNOWN
define|#
directive|define
name|LENGTH_UNKNOWN
value|(~0)
end_define
begin_define
DECL|macro|DATE_RFC2822_LEN
define|#
directive|define
name|DATE_RFC2822_LEN
value|31
end_define
begin_comment
comment|/* Create memory pool for log messages */
end_comment
begin_macro
name|obj_pool_gen
argument_list|(
argument|log
argument_list|,
argument|char
argument_list|,
literal|4096
argument_list|)
end_macro
begin_decl_stmt
DECL|variable|input
specifier|static
name|struct
name|line_buffer
name|input
init|=
name|LINE_BUFFER_INIT
decl_stmt|;
end_decl_stmt
begin_define
DECL|macro|REPORT_FILENO
define|#
directive|define
name|REPORT_FILENO
value|3
end_define
begin_function
DECL|function|log_copy
specifier|static
name|char
modifier|*
name|log_copy
parameter_list|(
name|uint32_t
name|length
parameter_list|,
specifier|const
name|char
modifier|*
name|log
parameter_list|)
block|{
name|char
modifier|*
name|buffer
decl_stmt|;
name|log_free
argument_list|(
name|log_pool
operator|.
name|size
argument_list|)
expr_stmt|;
name|buffer
operator|=
name|log_pointer
argument_list|(
name|log_alloc
argument_list|(
name|length
argument_list|)
argument_list|)
expr_stmt|;
name|strncpy
argument_list|(
name|buffer
argument_list|,
name|log
argument_list|,
name|length
argument_list|)
expr_stmt|;
return|return
name|buffer
return|;
block|}
end_function
begin_struct
specifier|static
struct|struct
block|{
DECL|member|action
DECL|member|propLength
DECL|member|textLength
DECL|member|srcRev
DECL|member|type
name|uint32_t
name|action
decl_stmt|,
name|propLength
decl_stmt|,
name|textLength
decl_stmt|,
name|srcRev
decl_stmt|,
name|type
decl_stmt|;
DECL|member|src
DECL|member|dst
name|uint32_t
name|src
index|[
name|REPO_MAX_PATH_DEPTH
index|]
decl_stmt|,
name|dst
index|[
name|REPO_MAX_PATH_DEPTH
index|]
decl_stmt|;
DECL|member|text_delta
DECL|member|prop_delta
name|uint32_t
name|text_delta
decl_stmt|,
name|prop_delta
decl_stmt|;
block|}
DECL|variable|node_ctx
name|node_ctx
struct|;
end_struct
begin_struct
specifier|static
struct|struct
block|{
DECL|member|revision
DECL|member|author
name|uint32_t
name|revision
decl_stmt|,
name|author
decl_stmt|;
DECL|member|timestamp
name|unsigned
name|long
name|timestamp
decl_stmt|;
DECL|member|log
name|char
modifier|*
name|log
decl_stmt|;
block|}
DECL|variable|rev_ctx
name|rev_ctx
struct|;
end_struct
begin_struct
specifier|static
struct|struct
block|{
DECL|member|version
DECL|member|uuid
DECL|member|url
name|uint32_t
name|version
decl_stmt|,
name|uuid
decl_stmt|,
name|url
decl_stmt|;
block|}
DECL|variable|dump_ctx
name|dump_ctx
struct|;
end_struct
begin_struct
specifier|static
struct|struct
block|{
DECL|member|svn_log
DECL|member|svn_author
DECL|member|svn_date
DECL|member|svn_executable
DECL|member|svn_special
DECL|member|uuid
name|uint32_t
name|svn_log
decl_stmt|,
name|svn_author
decl_stmt|,
name|svn_date
decl_stmt|,
name|svn_executable
decl_stmt|,
name|svn_special
decl_stmt|,
name|uuid
decl_stmt|,
DECL|member|revision_number
DECL|member|node_path
DECL|member|node_kind
DECL|member|node_action
name|revision_number
decl_stmt|,
name|node_path
decl_stmt|,
name|node_kind
decl_stmt|,
name|node_action
decl_stmt|,
DECL|member|node_copyfrom_path
DECL|member|node_copyfrom_rev
DECL|member|text_content_length
name|node_copyfrom_path
decl_stmt|,
name|node_copyfrom_rev
decl_stmt|,
name|text_content_length
decl_stmt|,
DECL|member|prop_content_length
DECL|member|content_length
DECL|member|svn_fs_dump_format_version
name|prop_content_length
decl_stmt|,
name|content_length
decl_stmt|,
name|svn_fs_dump_format_version
decl_stmt|,
comment|/* version 3 format */
DECL|member|text_delta
DECL|member|prop_delta
name|text_delta
decl_stmt|,
name|prop_delta
decl_stmt|;
block|}
DECL|variable|keys
name|keys
struct|;
end_struct
begin_function
DECL|function|reset_node_ctx
specifier|static
name|void
name|reset_node_ctx
parameter_list|(
name|char
modifier|*
name|fname
parameter_list|)
block|{
name|node_ctx
operator|.
name|type
operator|=
literal|0
expr_stmt|;
name|node_ctx
operator|.
name|action
operator|=
name|NODEACT_UNKNOWN
expr_stmt|;
name|node_ctx
operator|.
name|propLength
operator|=
name|LENGTH_UNKNOWN
expr_stmt|;
name|node_ctx
operator|.
name|textLength
operator|=
name|LENGTH_UNKNOWN
expr_stmt|;
name|node_ctx
operator|.
name|src
index|[
literal|0
index|]
operator|=
operator|~
literal|0
expr_stmt|;
name|node_ctx
operator|.
name|srcRev
operator|=
literal|0
expr_stmt|;
name|pool_tok_seq
argument_list|(
name|REPO_MAX_PATH_DEPTH
argument_list|,
name|node_ctx
operator|.
name|dst
argument_list|,
literal|"/"
argument_list|,
name|fname
argument_list|)
expr_stmt|;
name|node_ctx
operator|.
name|text_delta
operator|=
literal|0
expr_stmt|;
name|node_ctx
operator|.
name|prop_delta
operator|=
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|reset_rev_ctx
specifier|static
name|void
name|reset_rev_ctx
parameter_list|(
name|uint32_t
name|revision
parameter_list|)
block|{
name|rev_ctx
operator|.
name|revision
operator|=
name|revision
expr_stmt|;
name|rev_ctx
operator|.
name|timestamp
operator|=
literal|0
expr_stmt|;
name|rev_ctx
operator|.
name|log
operator|=
name|NULL
expr_stmt|;
name|rev_ctx
operator|.
name|author
operator|=
operator|~
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|reset_dump_ctx
specifier|static
name|void
name|reset_dump_ctx
parameter_list|(
name|uint32_t
name|url
parameter_list|)
block|{
name|dump_ctx
operator|.
name|url
operator|=
name|url
expr_stmt|;
name|dump_ctx
operator|.
name|version
operator|=
literal|1
expr_stmt|;
name|dump_ctx
operator|.
name|uuid
operator|=
operator|~
literal|0
expr_stmt|;
block|}
end_function
begin_function
DECL|function|init_keys
specifier|static
name|void
name|init_keys
parameter_list|(
name|void
parameter_list|)
block|{
name|keys
operator|.
name|svn_log
operator|=
name|pool_intern
argument_list|(
literal|"svn:log"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|svn_author
operator|=
name|pool_intern
argument_list|(
literal|"svn:author"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|svn_date
operator|=
name|pool_intern
argument_list|(
literal|"svn:date"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|svn_executable
operator|=
name|pool_intern
argument_list|(
literal|"svn:executable"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|svn_special
operator|=
name|pool_intern
argument_list|(
literal|"svn:special"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|uuid
operator|=
name|pool_intern
argument_list|(
literal|"UUID"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|revision_number
operator|=
name|pool_intern
argument_list|(
literal|"Revision-number"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|node_path
operator|=
name|pool_intern
argument_list|(
literal|"Node-path"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|node_kind
operator|=
name|pool_intern
argument_list|(
literal|"Node-kind"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|node_action
operator|=
name|pool_intern
argument_list|(
literal|"Node-action"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|node_copyfrom_path
operator|=
name|pool_intern
argument_list|(
literal|"Node-copyfrom-path"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|node_copyfrom_rev
operator|=
name|pool_intern
argument_list|(
literal|"Node-copyfrom-rev"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|text_content_length
operator|=
name|pool_intern
argument_list|(
literal|"Text-content-length"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|prop_content_length
operator|=
name|pool_intern
argument_list|(
literal|"Prop-content-length"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|content_length
operator|=
name|pool_intern
argument_list|(
literal|"Content-length"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|svn_fs_dump_format_version
operator|=
name|pool_intern
argument_list|(
literal|"SVN-fs-dump-format-version"
argument_list|)
expr_stmt|;
comment|/* version 3 format (Subversion 1.1.0) */
name|keys
operator|.
name|text_delta
operator|=
name|pool_intern
argument_list|(
literal|"Text-delta"
argument_list|)
expr_stmt|;
name|keys
operator|.
name|prop_delta
operator|=
name|pool_intern
argument_list|(
literal|"Prop-delta"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|handle_property
specifier|static
name|void
name|handle_property
parameter_list|(
name|uint32_t
name|key
parameter_list|,
specifier|const
name|char
modifier|*
name|val
parameter_list|,
name|uint32_t
name|len
parameter_list|,
name|uint32_t
modifier|*
name|type_set
parameter_list|)
block|{
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|svn_log
condition|)
block|{
if|if
condition|(
operator|!
name|val
condition|)
name|die
argument_list|(
literal|"invalid dump: unsets svn:log"
argument_list|)
expr_stmt|;
comment|/* Value length excludes terminating nul. */
name|rev_ctx
operator|.
name|log
operator|=
name|log_copy
argument_list|(
name|len
operator|+
literal|1
argument_list|,
name|val
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|svn_author
condition|)
block|{
name|rev_ctx
operator|.
name|author
operator|=
name|pool_intern
argument_list|(
name|val
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|svn_date
condition|)
block|{
if|if
condition|(
operator|!
name|val
condition|)
name|die
argument_list|(
literal|"invalid dump: unsets svn:date"
argument_list|)
expr_stmt|;
if|if
condition|(
name|parse_date_basic
argument_list|(
name|val
argument_list|,
operator|&
name|rev_ctx
operator|.
name|timestamp
argument_list|,
name|NULL
argument_list|)
condition|)
name|warning
argument_list|(
literal|"invalid timestamp: %s"
argument_list|,
name|val
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|svn_executable
operator|||
name|key
operator|==
name|keys
operator|.
name|svn_special
condition|)
block|{
if|if
condition|(
operator|*
name|type_set
condition|)
block|{
if|if
condition|(
operator|!
name|val
condition|)
return|return;
name|die
argument_list|(
literal|"invalid dump: sets type twice"
argument_list|)
expr_stmt|;
block|}
if|if
condition|(
operator|!
name|val
condition|)
block|{
name|node_ctx
operator|.
name|type
operator|=
name|REPO_MODE_BLB
expr_stmt|;
return|return;
block|}
operator|*
name|type_set
operator|=
literal|1
expr_stmt|;
name|node_ctx
operator|.
name|type
operator|=
name|key
operator|==
name|keys
operator|.
name|svn_executable
condition|?
name|REPO_MODE_EXE
else|:
name|REPO_MODE_LNK
expr_stmt|;
block|}
block|}
end_function
begin_function
DECL|function|read_props
specifier|static
name|void
name|read_props
parameter_list|(
name|void
parameter_list|)
block|{
name|uint32_t
name|key
init|=
operator|~
literal|0
decl_stmt|;
specifier|const
name|char
modifier|*
name|t
decl_stmt|;
comment|/* 	 * NEEDSWORK: to support simple mode changes like 	 *	K 11 	 *	svn:special 	 *	V 1 	 *	* 	 *	D 14 	 *	svn:executable 	 * we keep track of whether a mode has been set and reset to 	 * plain file only if not.  We should be keeping track of the 	 * symlink and executable bits separately instead. 	 */
name|uint32_t
name|type_set
init|=
literal|0
decl_stmt|;
while|while
condition|(
operator|(
name|t
operator|=
name|buffer_read_line
argument_list|(
operator|&
name|input
argument_list|)
operator|)
operator|&&
name|strcmp
argument_list|(
name|t
argument_list|,
literal|"PROPS-END"
argument_list|)
condition|)
block|{
name|uint32_t
name|len
decl_stmt|;
specifier|const
name|char
modifier|*
name|val
decl_stmt|;
specifier|const
name|char
name|type
init|=
name|t
index|[
literal|0
index|]
decl_stmt|;
if|if
condition|(
operator|!
name|type
operator|||
name|t
index|[
literal|1
index|]
operator|!=
literal|' '
condition|)
name|die
argument_list|(
literal|"invalid property line: %s\n"
argument_list|,
name|t
argument_list|)
expr_stmt|;
name|len
operator|=
name|atoi
argument_list|(
operator|&
name|t
index|[
literal|2
index|]
argument_list|)
expr_stmt|;
name|val
operator|=
name|buffer_read_string
argument_list|(
operator|&
name|input
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|buffer_skip_bytes
argument_list|(
operator|&
name|input
argument_list|,
literal|1
argument_list|)
expr_stmt|;
comment|/* Discard trailing newline. */
switch|switch
condition|(
name|type
condition|)
block|{
case|case
literal|'K'
case|:
name|key
operator|=
name|pool_intern
argument_list|(
name|val
argument_list|)
expr_stmt|;
continue|continue;
case|case
literal|'D'
case|:
name|key
operator|=
name|pool_intern
argument_list|(
name|val
argument_list|)
expr_stmt|;
name|val
operator|=
name|NULL
expr_stmt|;
name|len
operator|=
literal|0
expr_stmt|;
comment|/* fall through */
case|case
literal|'V'
case|:
name|handle_property
argument_list|(
name|key
argument_list|,
name|val
argument_list|,
name|len
argument_list|,
operator|&
name|type_set
argument_list|)
expr_stmt|;
name|key
operator|=
operator|~
literal|0
expr_stmt|;
continue|continue;
default|default:
name|die
argument_list|(
literal|"invalid property line: %s\n"
argument_list|,
name|t
argument_list|)
expr_stmt|;
block|}
block|}
block|}
end_function
begin_function
DECL|function|handle_node
specifier|static
name|void
name|handle_node
parameter_list|(
name|void
parameter_list|)
block|{
specifier|const
name|uint32_t
name|type
init|=
name|node_ctx
operator|.
name|type
decl_stmt|;
specifier|const
name|int
name|have_props
init|=
name|node_ctx
operator|.
name|propLength
operator|!=
name|LENGTH_UNKNOWN
decl_stmt|;
specifier|const
name|int
name|have_text
init|=
name|node_ctx
operator|.
name|textLength
operator|!=
name|LENGTH_UNKNOWN
decl_stmt|;
comment|/* 	 * Old text for this node: 	 *  NULL	- directory or bug 	 *  empty_blob	- empty 	 *  "<dataref>"	- data retrievable from fast-import 	 */
specifier|static
specifier|const
name|char
modifier|*
specifier|const
name|empty_blob
init|=
literal|"::empty::"
decl_stmt|;
specifier|const
name|char
modifier|*
name|old_data
init|=
name|NULL
decl_stmt|;
if|if
condition|(
name|node_ctx
operator|.
name|text_delta
condition|)
name|die
argument_list|(
literal|"text deltas not supported"
argument_list|)
expr_stmt|;
if|if
condition|(
name|node_ctx
operator|.
name|action
operator|==
name|NODEACT_DELETE
condition|)
block|{
if|if
condition|(
name|have_text
operator|||
name|have_props
operator|||
name|node_ctx
operator|.
name|srcRev
condition|)
name|die
argument_list|(
literal|"invalid dump: deletion node has "
literal|"copyfrom info, text, or properties"
argument_list|)
expr_stmt|;
return|return
name|repo_delete
argument_list|(
name|node_ctx
operator|.
name|dst
argument_list|)
return|;
block|}
if|if
condition|(
name|node_ctx
operator|.
name|action
operator|==
name|NODEACT_REPLACE
condition|)
block|{
name|repo_delete
argument_list|(
name|node_ctx
operator|.
name|dst
argument_list|)
expr_stmt|;
name|node_ctx
operator|.
name|action
operator|=
name|NODEACT_ADD
expr_stmt|;
block|}
if|if
condition|(
name|node_ctx
operator|.
name|srcRev
condition|)
block|{
name|repo_copy
argument_list|(
name|node_ctx
operator|.
name|srcRev
argument_list|,
name|node_ctx
operator|.
name|src
argument_list|,
name|node_ctx
operator|.
name|dst
argument_list|)
expr_stmt|;
if|if
condition|(
name|node_ctx
operator|.
name|action
operator|==
name|NODEACT_ADD
condition|)
name|node_ctx
operator|.
name|action
operator|=
name|NODEACT_CHANGE
expr_stmt|;
block|}
if|if
condition|(
name|have_text
operator|&&
name|type
operator|==
name|REPO_MODE_DIR
condition|)
name|die
argument_list|(
literal|"invalid dump: directories cannot have text attached"
argument_list|)
expr_stmt|;
comment|/* 	 * Find old content (old_data) and decide on the new mode. 	 */
if|if
condition|(
name|node_ctx
operator|.
name|action
operator|==
name|NODEACT_CHANGE
operator|&&
operator|!
operator|~
operator|*
name|node_ctx
operator|.
name|dst
condition|)
block|{
if|if
condition|(
name|type
operator|!=
name|REPO_MODE_DIR
condition|)
name|die
argument_list|(
literal|"invalid dump: root of tree is not a regular file"
argument_list|)
expr_stmt|;
name|old_data
operator|=
name|NULL
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|node_ctx
operator|.
name|action
operator|==
name|NODEACT_CHANGE
condition|)
block|{
name|uint32_t
name|mode
decl_stmt|;
name|old_data
operator|=
name|repo_read_path
argument_list|(
name|node_ctx
operator|.
name|dst
argument_list|,
operator|&
name|mode
argument_list|)
expr_stmt|;
if|if
condition|(
name|mode
operator|==
name|REPO_MODE_DIR
operator|&&
name|type
operator|!=
name|REPO_MODE_DIR
condition|)
name|die
argument_list|(
literal|"invalid dump: cannot modify a directory into a file"
argument_list|)
expr_stmt|;
if|if
condition|(
name|mode
operator|!=
name|REPO_MODE_DIR
operator|&&
name|type
operator|==
name|REPO_MODE_DIR
condition|)
name|die
argument_list|(
literal|"invalid dump: cannot modify a file into a directory"
argument_list|)
expr_stmt|;
name|node_ctx
operator|.
name|type
operator|=
name|mode
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|node_ctx
operator|.
name|action
operator|==
name|NODEACT_ADD
condition|)
block|{
if|if
condition|(
name|type
operator|==
name|REPO_MODE_DIR
condition|)
name|old_data
operator|=
name|NULL
expr_stmt|;
elseif|else
if|if
condition|(
name|have_text
condition|)
name|old_data
operator|=
name|empty_blob
expr_stmt|;
else|else
name|die
argument_list|(
literal|"invalid dump: adds node without text"
argument_list|)
expr_stmt|;
block|}
else|else
block|{
name|die
argument_list|(
literal|"invalid dump: Node-path block lacks Node-action"
argument_list|)
expr_stmt|;
block|}
comment|/* 	 * Adjust mode to reflect properties. 	 */
if|if
condition|(
name|have_props
condition|)
block|{
if|if
condition|(
operator|!
name|node_ctx
operator|.
name|prop_delta
condition|)
name|node_ctx
operator|.
name|type
operator|=
name|type
expr_stmt|;
if|if
condition|(
name|node_ctx
operator|.
name|propLength
condition|)
name|read_props
argument_list|()
expr_stmt|;
block|}
comment|/* 	 * Save the result. 	 */
if|if
condition|(
name|type
operator|==
name|REPO_MODE_DIR
condition|)
comment|/* directories are not tracked. */
return|return;
name|assert
argument_list|(
name|old_data
argument_list|)
expr_stmt|;
if|if
condition|(
name|old_data
operator|==
name|empty_blob
condition|)
comment|/* For the fast_export_* functions, NULL means empty. */
name|old_data
operator|=
name|NULL
expr_stmt|;
if|if
condition|(
operator|!
name|have_text
condition|)
block|{
name|fast_export_modify
argument_list|(
name|REPO_MAX_PATH_DEPTH
argument_list|,
name|node_ctx
operator|.
name|dst
argument_list|,
name|node_ctx
operator|.
name|type
argument_list|,
name|old_data
argument_list|)
expr_stmt|;
return|return;
block|}
name|fast_export_modify
argument_list|(
name|REPO_MAX_PATH_DEPTH
argument_list|,
name|node_ctx
operator|.
name|dst
argument_list|,
name|node_ctx
operator|.
name|type
argument_list|,
literal|"inline"
argument_list|)
expr_stmt|;
name|fast_export_data
argument_list|(
name|node_ctx
operator|.
name|type
argument_list|,
name|node_ctx
operator|.
name|textLength
argument_list|,
operator|&
name|input
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|begin_revision
specifier|static
name|void
name|begin_revision
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
operator|!
name|rev_ctx
operator|.
name|revision
condition|)
comment|/* revision 0 gets no git commit. */
return|return;
name|fast_export_begin_commit
argument_list|(
name|rev_ctx
operator|.
name|revision
argument_list|,
name|rev_ctx
operator|.
name|author
argument_list|,
name|rev_ctx
operator|.
name|log
argument_list|,
name|dump_ctx
operator|.
name|uuid
argument_list|,
name|dump_ctx
operator|.
name|url
argument_list|,
name|rev_ctx
operator|.
name|timestamp
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|end_revision
specifier|static
name|void
name|end_revision
parameter_list|(
name|void
parameter_list|)
block|{
if|if
condition|(
name|rev_ctx
operator|.
name|revision
condition|)
name|fast_export_end_commit
argument_list|(
name|rev_ctx
operator|.
name|revision
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|svndump_read
name|void
name|svndump_read
parameter_list|(
specifier|const
name|char
modifier|*
name|url
parameter_list|)
block|{
name|char
modifier|*
name|val
decl_stmt|;
name|char
modifier|*
name|t
decl_stmt|;
name|uint32_t
name|active_ctx
init|=
name|DUMP_CTX
decl_stmt|;
name|uint32_t
name|len
decl_stmt|;
name|uint32_t
name|key
decl_stmt|;
name|reset_dump_ctx
argument_list|(
name|pool_intern
argument_list|(
name|url
argument_list|)
argument_list|)
expr_stmt|;
while|while
condition|(
operator|(
name|t
operator|=
name|buffer_read_line
argument_list|(
operator|&
name|input
argument_list|)
operator|)
condition|)
block|{
name|val
operator|=
name|strstr
argument_list|(
name|t
argument_list|,
literal|": "
argument_list|)
expr_stmt|;
if|if
condition|(
operator|!
name|val
condition|)
continue|continue;
operator|*
name|val
operator|++
operator|=
literal|'\0'
expr_stmt|;
operator|*
name|val
operator|++
operator|=
literal|'\0'
expr_stmt|;
name|key
operator|=
name|pool_intern
argument_list|(
name|t
argument_list|)
expr_stmt|;
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|svn_fs_dump_format_version
condition|)
block|{
name|dump_ctx
operator|.
name|version
operator|=
name|atoi
argument_list|(
name|val
argument_list|)
expr_stmt|;
if|if
condition|(
name|dump_ctx
operator|.
name|version
operator|>
literal|3
condition|)
name|die
argument_list|(
literal|"expected svn dump format version<= 3, found %"
name|PRIu32
argument_list|,
name|dump_ctx
operator|.
name|version
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|uuid
condition|)
block|{
name|dump_ctx
operator|.
name|uuid
operator|=
name|pool_intern
argument_list|(
name|val
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|revision_number
condition|)
block|{
if|if
condition|(
name|active_ctx
operator|==
name|NODE_CTX
condition|)
name|handle_node
argument_list|()
expr_stmt|;
if|if
condition|(
name|active_ctx
operator|==
name|REV_CTX
condition|)
name|begin_revision
argument_list|()
expr_stmt|;
if|if
condition|(
name|active_ctx
operator|!=
name|DUMP_CTX
condition|)
name|end_revision
argument_list|()
expr_stmt|;
name|active_ctx
operator|=
name|REV_CTX
expr_stmt|;
name|reset_rev_ctx
argument_list|(
name|atoi
argument_list|(
name|val
argument_list|)
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|node_path
condition|)
block|{
if|if
condition|(
name|active_ctx
operator|==
name|NODE_CTX
condition|)
name|handle_node
argument_list|()
expr_stmt|;
if|if
condition|(
name|active_ctx
operator|==
name|REV_CTX
condition|)
name|begin_revision
argument_list|()
expr_stmt|;
name|active_ctx
operator|=
name|NODE_CTX
expr_stmt|;
name|reset_node_ctx
argument_list|(
name|val
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|node_kind
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|val
argument_list|,
literal|"dir"
argument_list|)
condition|)
name|node_ctx
operator|.
name|type
operator|=
name|REPO_MODE_DIR
expr_stmt|;
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|val
argument_list|,
literal|"file"
argument_list|)
condition|)
name|node_ctx
operator|.
name|type
operator|=
name|REPO_MODE_BLB
expr_stmt|;
else|else
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Unknown node-kind: %s\n"
argument_list|,
name|val
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|node_action
condition|)
block|{
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|val
argument_list|,
literal|"delete"
argument_list|)
condition|)
block|{
name|node_ctx
operator|.
name|action
operator|=
name|NODEACT_DELETE
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|val
argument_list|,
literal|"add"
argument_list|)
condition|)
block|{
name|node_ctx
operator|.
name|action
operator|=
name|NODEACT_ADD
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|val
argument_list|,
literal|"change"
argument_list|)
condition|)
block|{
name|node_ctx
operator|.
name|action
operator|=
name|NODEACT_CHANGE
expr_stmt|;
block|}
elseif|else
if|if
condition|(
operator|!
name|strcmp
argument_list|(
name|val
argument_list|,
literal|"replace"
argument_list|)
condition|)
block|{
name|node_ctx
operator|.
name|action
operator|=
name|NODEACT_REPLACE
expr_stmt|;
block|}
else|else
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Unknown node-action: %s\n"
argument_list|,
name|val
argument_list|)
expr_stmt|;
name|node_ctx
operator|.
name|action
operator|=
name|NODEACT_UNKNOWN
expr_stmt|;
block|}
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|node_copyfrom_path
condition|)
block|{
name|pool_tok_seq
argument_list|(
name|REPO_MAX_PATH_DEPTH
argument_list|,
name|node_ctx
operator|.
name|src
argument_list|,
literal|"/"
argument_list|,
name|val
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|node_copyfrom_rev
condition|)
block|{
name|node_ctx
operator|.
name|srcRev
operator|=
name|atoi
argument_list|(
name|val
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|text_content_length
condition|)
block|{
name|node_ctx
operator|.
name|textLength
operator|=
name|atoi
argument_list|(
name|val
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|prop_content_length
condition|)
block|{
name|node_ctx
operator|.
name|propLength
operator|=
name|atoi
argument_list|(
name|val
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|text_delta
condition|)
block|{
name|node_ctx
operator|.
name|text_delta
operator|=
operator|!
name|strcmp
argument_list|(
name|val
argument_list|,
literal|"true"
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|prop_delta
condition|)
block|{
name|node_ctx
operator|.
name|prop_delta
operator|=
operator|!
name|strcmp
argument_list|(
name|val
argument_list|,
literal|"true"
argument_list|)
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|key
operator|==
name|keys
operator|.
name|content_length
condition|)
block|{
name|len
operator|=
name|atoi
argument_list|(
name|val
argument_list|)
expr_stmt|;
name|buffer_read_line
argument_list|(
operator|&
name|input
argument_list|)
expr_stmt|;
if|if
condition|(
name|active_ctx
operator|==
name|REV_CTX
condition|)
block|{
name|read_props
argument_list|()
expr_stmt|;
block|}
elseif|else
if|if
condition|(
name|active_ctx
operator|==
name|NODE_CTX
condition|)
block|{
name|handle_node
argument_list|()
expr_stmt|;
name|active_ctx
operator|=
name|INTERNODE_CTX
expr_stmt|;
block|}
else|else
block|{
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Unexpected content length header: %"
name|PRIu32
literal|"\n"
argument_list|,
name|len
argument_list|)
expr_stmt|;
name|buffer_skip_bytes
argument_list|(
operator|&
name|input
argument_list|,
name|len
argument_list|)
expr_stmt|;
block|}
block|}
block|}
if|if
condition|(
name|active_ctx
operator|==
name|NODE_CTX
condition|)
name|handle_node
argument_list|()
expr_stmt|;
if|if
condition|(
name|active_ctx
operator|==
name|REV_CTX
condition|)
name|begin_revision
argument_list|()
expr_stmt|;
if|if
condition|(
name|active_ctx
operator|!=
name|DUMP_CTX
condition|)
name|end_revision
argument_list|()
expr_stmt|;
block|}
end_function
begin_function
DECL|function|svndump_init
name|int
name|svndump_init
parameter_list|(
specifier|const
name|char
modifier|*
name|filename
parameter_list|)
block|{
if|if
condition|(
name|buffer_init
argument_list|(
operator|&
name|input
argument_list|,
name|filename
argument_list|)
condition|)
return|return
name|error
argument_list|(
literal|"cannot open %s: %s"
argument_list|,
name|filename
argument_list|,
name|strerror
argument_list|(
name|errno
argument_list|)
argument_list|)
return|;
name|fast_export_init
argument_list|(
name|REPORT_FILENO
argument_list|)
expr_stmt|;
name|reset_dump_ctx
argument_list|(
operator|~
literal|0
argument_list|)
expr_stmt|;
name|reset_rev_ctx
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|reset_node_ctx
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
name|init_keys
argument_list|()
expr_stmt|;
return|return
literal|0
return|;
block|}
end_function
begin_function
DECL|function|svndump_deinit
name|void
name|svndump_deinit
parameter_list|(
name|void
parameter_list|)
block|{
name|log_reset
argument_list|()
expr_stmt|;
name|fast_export_deinit
argument_list|()
expr_stmt|;
name|reset_dump_ctx
argument_list|(
operator|~
literal|0
argument_list|)
expr_stmt|;
name|reset_rev_ctx
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|reset_node_ctx
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
if|if
condition|(
name|buffer_deinit
argument_list|(
operator|&
name|input
argument_list|)
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Input error\n"
argument_list|)
expr_stmt|;
if|if
condition|(
name|ferror
argument_list|(
name|stdout
argument_list|)
condition|)
name|fprintf
argument_list|(
name|stderr
argument_list|,
literal|"Output error\n"
argument_list|)
expr_stmt|;
block|}
end_function
begin_function
DECL|function|svndump_reset
name|void
name|svndump_reset
parameter_list|(
name|void
parameter_list|)
block|{
name|log_reset
argument_list|()
expr_stmt|;
name|fast_export_reset
argument_list|()
expr_stmt|;
name|buffer_reset
argument_list|(
operator|&
name|input
argument_list|)
expr_stmt|;
name|reset_dump_ctx
argument_list|(
operator|~
literal|0
argument_list|)
expr_stmt|;
name|reset_rev_ctx
argument_list|(
literal|0
argument_list|)
expr_stmt|;
name|reset_node_ctx
argument_list|(
name|NULL
argument_list|)
expr_stmt|;
block|}
end_function
end_unit
