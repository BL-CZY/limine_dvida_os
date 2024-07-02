#ifndef EXT2_STRUCTS_H
#define EXT2_STRUCTS_H

#include <stdint.h>
#include "mod/algorithms/guid.h"

// *everything little endian unless otherwise stated

//* file format
#define EXT2_S_IFSOCK	0xC000	// socket
#define EXT2_S_IFLNK	0xA000	// symbolic link
#define EXT2_S_IFREG	0x8000	// regular file
#define EXT2_S_IFBLK	0x6000	// block device
#define EXT2_S_IFDIR	0x4000	// directory
#define EXT2_S_IFCHR	0x2000	// character device
#define EXT2_S_IFIFO	0x1000	// fifo

//* process execution user/group override
#define EXT2_S_ISUID	0x0800	// Set process User ID
#define EXT2_S_ISGID	0x0400	// Set process Group ID
#define EXT2_S_ISVTX	0x0200	// sticky bit

//* access rights
#define EXT2_S_IRUSR	0x0100	// user read
#define EXT2_S_IWUSR	0x0080	// user write
#define EXT2_S_IXUSR	0x0040	// user execute
#define EXT2_S_IRGRP	0x0020	// group read
#define EXT2_S_IWGRP	0x0010	// group write
#define EXT2_S_IXGRP	0x0008	// group execute
#define EXT2_S_IROTH	0x0004	// others read
#define EXT2_S_IWOTH	0x0002	// others write
#define EXT2_S_IXOTH	0x0001	// others execute

#define EXT2_SECRM_FL	0x00000001	// secure deletion
#define EXT2_UNRM_FL	0x00000002	// record for undelete
#define EXT2_COMPR_FL	0x00000004	// compressed file
#define EXT2_SYNC_FL	0x00000008	// synchronous updates
#define EXT2_IMMUTABLE_FL	0x00000010	// immutable file
#define EXT2_APPEND_FL	0x00000020	// append only
#define EXT2_NODUMP_FL	0x00000040	// do not dump/delete file
#define EXT2_NOATIME_FL	0x00000080	// do not update .i_atime

//* Reserved for compression usage
#define EXT2_DIRTY_FL	0x00000100	// Dirty (modified)
#define EXT2_COMPRBLK_FL	0x00000200	// compressed blocks
#define EXT2_NOCOMPR_FL	0x00000400	// access raw compressed data
#define EXT2_ECOMPR_FL	0x00000800	// compression error

//* End of compression flags
#define EXT2_BTREE_FL	0x00001000	// b-tree format directory
#define EXT2_INDEX_FL	0x00001000	// hash indexed directory
#define EXT2_IMAGIC_FL	0x00002000	// AFS directory
#define EXT3_JOURNAL_DATA_FL	0x00004000	// journal file data
#define EXT2_RESERVED_FL	0x80000000	// reserved for ext2 library

typedef struct ext2_superblock {
    #pragma region basic

    uint32_t s_inodes_count;
    uint32_t s_blocks_count;
    uint32_t s_free_blocks_count;
    uint32_t s_free_inodes_count;
    uint32_t s_first_data_block;
    uint32_t s_log_block_size;
    uint32_t s_log_frag_size;
    uint32_t s_blocks_per_group;
    uint32_t s_frags_per_group;
    uint32_t s_inodes_per_group;
    uint32_t s_mtime;
    uint32_t s_wtime;
    uint16_t s_mnt_count;
    uint16_t s_max_mnt_count;
    uint16_t s_magic;
    uint16_t s_state;
    uint16_t s_errors;
    uint16_t s_minor_rev_level;
    uint32_t s_lastcheck;
    uint32_t s_checkinterval;
    uint32_t s_creator_os;
    uint32_t s_rev_level;
    uint16_t s_def_resuid;
    uint16_t s_def_resgid;

    #pragma endregion

    #pragma region EXT2_DYNAMIC_REV 
    
    uint32_t s_first_ino;
    uint16_t s_inode_size;
    uint16_t s_block_group_nr;
    uint32_t s_feature_compat;
    uint32_t s_feature_incompat;
    uint32_t s_feature_ro_compat;
    guid_t s_uuid;
    uint8_t s_volume_name[16];
    uint8_t s_last_mounted[64];
    uint32_t s_algo_bitmap;

    #pragma endregion

    #pragma region Performance Hints

    uint8_t s_prealloc_blocks;
    uint8_t s_prealloc_dir_blocks;
    //* uint8_t padding[2];

    #pragma endregion

    #pragma region Jounaling support

    guid_t s_journal_uuid;
    uint32_t s_journal_inum;
    uint32_t s_journal_dev;
    uint32_t s_last_orphan;

    #pragma endregion

    #pragma region Directory Index Support

    uint32_t s_hash_seed[4];
    uint8_t s_def_hash_version;
    //* uint8_t padding[3];

    #pragma endregion

    #pragma region Other options

    uint32_t s_default_mount_options;
    uint32_t s_first_meta_bg;

    #pragma endregion
} ext2_superblock_t;


typedef struct ext2_blockgrp_descriptor_table {
    uint32_t bg_block_bitmap;
    uint32_t bg_inode_bitmap;
    uint32_t bg_inode_table;
    uint16_t bg_free_blocks_count;
    uint16_t bg_free_inodes_count;
    uint16_t bg_used_dirs_count;
    uint16_t bg_pad;
    uint8_t reserved[12];
} ext2_blockgrp_descriptor_table_t;

typedef struct inode {
    /**
     * the i_mode consists of three part
     * the top 4 bits represent the file type, socket, symlink, ...
     * the following 3 bits represent process execution/user overrides
     * the last 9 bits represent access rights
     */
    uint16_t i_mode;
    uint16_t i_uid;
    uint32_t i_size;
    uint32_t i_atime;
    uint32_t i_ctime;
    uint32_t i_mtime;
    uint32_t i_dtime;
    uint16_t i_gid;
    uint16_t i_links_count;
    uint32_t i_blocks;
    /**
     * 32 bits value for flags
     */
    uint32_t i_flags;
    uint32_t i_osd1;
    uint32_t i_block[15];
    uint32_t i_generation;
    uint32_t i_file_acl;
    uint32_t i_dir_acl;
    uint32_t i_faddr;
    uint8_t i_osd2[12];
} inode_t;

#endif