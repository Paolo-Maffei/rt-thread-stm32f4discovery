/*
 * File      : dfs_posix.c
 * This file is part of Device File System in RT-Thread RTOS
 * COPYRIGHT (C) 2004-2012, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE.
 *
 * Change Logs:
 * Date           Author       Notes
 * 2009-05-27     Yi.qiu       The first version
 */

#include <dfs.h>
#include <dfs_posix.h>

/**
 * @addtogroup FsPosixApi
 */
/*@{*/

/**
 * this function is a POSIX compliant version, which will open a file and return
 * a file descriptor.
 *
 * @param file the path name of file.
 * @param flags the file open flags.
 * @param mode
 *
 * @return the non-negative integer on successful open, others for failed.
 */
int open(const char *file, int flags, int mode)
{
	int fd, result;
	struct dfs_fd *d;

	/* allocate a fd */
	fd = fd_new();
	if (fd < 0)
		return -1;
	d  = fd_get(fd);

	result = dfs_file_open(d, file, flags);
	if (result < 0)
	{
		/* release the ref-count of fd */
		fd_put(d);
		fd_put(d);
		
		rt_set_errno(result);

		return -1;
	}

	/* release the ref-count of fd */
	fd_put(d);
	return fd;
}

/**
 * this function is a POSIX compliant version, which will close the open
 * file descriptor.
 *
 * @param fd the file descriptor.
 *
 * @return 0 on successful, -1 on failed.
 */
int close(int fd)
{
	int result;
	struct dfs_fd *d;

	d = fd_get(fd);
	if (d == RT_NULL)
	{
		rt_set_errno(-RT_ERROR);
		return -1;
	}

	result = dfs_file_close(d);
	fd_put(d);

	if (result < 0)
	{
		rt_set_errno(result);
		return -1;
	}

	fd_put(d);
	return 0;
}

/**
 * this function is a POSIX compliant version, which will read specified data buffer 
 * length for an open file descriptor.
 * 
 * @param fd the file descriptor.
 * @param buf the buffer to save the read data.
 * @param len the maximal length of data buffer
 *
 * @return the actual read data buffer length
 */
int read(int fd, void *buf, size_t len)
{
	int result;
	struct dfs_fd *d;

	/* get the fd */
	d  = fd_get(fd);
	if (d == RT_NULL)
	{
		rt_set_errno(-RT_ERROR);
		return -1;
	}

	result = dfs_file_read(d, buf, len);
	if (result < 0)
	{
		fd_put(d);
		rt_set_errno(result);

		return -1;
	}

	/* release the ref-count of fd */
	fd_put(d);
	return result;
}

/**
 * this function is a POSIX compliant version, which will write specified data buffer
 * length for an open file descriptor.
 *
 * @param fd the file descriptor
 * @param buf the data buffer to be written.
 * @param len the data buffer length.
 *
 * @return the actual written data buffer length.
 */
int write(int fd, const void *buf, size_t len)
{
	int result;
	struct dfs_fd *d;

	/* get the fd */
	d  = fd_get(fd);
	if (d == RT_NULL)
	{
		rt_set_errno(-RT_ERROR);
		return -1;
	}

	result = dfs_file_write(d, buf, len);
	if (result < 0)
	{
		fd_put(d);
		rt_set_errno(result);

		return -1;
	}

	/* release the ref-count of fd */
	fd_put(d);
	return result;
}

/**
 * this function is a POSIX compliant version, which will seek the offset for an
 * open file descriptor.
 *
 * @param fd the file descriptor.
 * @param offset the offset to be seeked.
 * @param whence the directory of seek.
 *
 * @return the current file position, or -1 on failed.
 */
off_t lseek(int fd, off_t offset, int whence)
{
	int result;
	struct dfs_fd *d;

	d = fd_get(fd);
	if (d == RT_NULL)
	{
		rt_set_errno(-RT_ERROR);
		return -1;
	}

	switch (whence)
	{
	case DFS_SEEK_SET:
		break;

	case DFS_SEEK_CUR:
		offset += d->pos;
		break;

	case DFS_SEEK_END:
		offset += d->size;
		break;

	default:
		rt_set_errno(-DFS_STATUS_EINVAL);
		return -1;

	}

	if (offset < 0)
	{
		rt_set_errno(-DFS_STATUS_EINVAL);
		return -1;
	}
	result = dfs_file_lseek(d, offset);
	if (result < 0)
	{
		fd_put(d);
		rt_set_errno(result);
		return -1;
	}

	/* release the ref-count of fd */
	fd_put(d);
	return offset;
}

/**
 * this function is a POSIX compliant version, which will rename old file name to
 * new file name.
 *
 * @param old the old file name.
 * @param new the new file name.
 *
 * @return 0 on successful, -1 on failed.
 *
 * note: the old and new file name must be belong to a same file system.
 */
int rename(const char *old, const char *new)
{
	int result;

	result = dfs_file_rename(old, new);
	if (result < 0)
	{
		rt_set_errno(result);
		return -1;
	}
	return 0;
}

/**
 * this function is a POSIX compliant version, which will unlink (remove) a 
 * specified path file from file system.
 * 
 * @param pathname the specified path name to be unlinked.
 *
 * @return 0 on successful, -1 on failed.
 */
int unlink(const char *pathname)
{
	int result;

	result = dfs_file_unlink(pathname);
	if (result < 0)
	{
		rt_set_errno(result);
		return -1;
	}
	return 0;
}

/**
 * this function is a POSIX compliant version, which will get file information.
 * 
 * @param file the file name
 * @param buf the data buffer to save stat description.
 *
 * @return 0 on successful, -1 on failed.
 */
int stat(const char *file, struct stat *buf)
{
	int result;

	result = dfs_file_stat(file, buf);
	if (result < 0)
	{
		rt_set_errno(result);
		return -1;
	}
	return result;
}

/**
 * this function is a POSIX compliant version, which will get file status.
 *
 * @param fildes the file description
 * @param buf the data buffer to save stat description.
 */
int fstat(int fildes, struct stat *buf)
{
	struct dfs_fd *d;

	/* get the fd */
	d = fd_get(fildes);
	if (d == RT_NULL)
	{
		rt_set_errno(-RT_ERROR);
		return -1;
	}

	/* it's the root directory */
	buf->st_dev = 0;

	buf->st_mode = DFS_S_IFREG | DFS_S_IRUSR | DFS_S_IRGRP | DFS_S_IROTH |
			DFS_S_IWUSR | DFS_S_IWGRP | DFS_S_IWOTH;
	if (d->type == FT_DIRECTORY)
	{
		buf->st_mode &= ~DFS_S_IFREG;
		buf->st_mode |= DFS_S_IFDIR | DFS_S_IXUSR | DFS_S_IXGRP | DFS_S_IXOTH;
	}

	buf->st_size  = d->size;
	buf->st_mtime = 0;
	buf->st_blksize = 512;

	fd_put(d);

	return DFS_STATUS_OK;
}

/**
 * this function is a POSIX compliant version, which will return the 
 * information about a mounted file system.
 * 
 * @param path the path which mounted file system.
 * @param buf the buffer to save the returned information.
 *
 * @return 0 on successful, others on failed.
 */
int statfs(const char *path, struct statfs *buf)
{
	int result;

	result = dfs_statfs(path, buf);
	if (result < 0)
	{
		rt_set_errno(result);
		return -1;
	}

	return result;
}

/**
 * this function is a POSIX compliant version, which will make a directory
 *
 * @param path the directory path to be made.
 * @param mode 
 *
 * @return 0 on successful, others on failed.
 */
int mkdir(const char *path, mode_t mode)
{
	int fd;
	struct dfs_fd *d;
	int result;

	fd = fd_new();
	if (fd == -1)
	{
		rt_kprintf("no fd\n");
		return -1;
	}

	d = fd_get(fd);

	result = dfs_file_open(d, path, DFS_O_DIRECTORY | DFS_O_CREAT);

	if (result < 0)
	{
		fd_put(d);
		rt_set_errno(result);
		return -1;
	}

	dfs_file_close(d);
	fd_put(d);
	return 0;
}
#ifdef RT_USING_FINSH
#include <finsh.h>
FINSH_FUNCTION_EXPORT(mkdir, create a directory);
#endif

/**
 * this function is a POSIX compliant version, which will remove a directory.
 *
 * @param pathname the path name to be removed.
 * 
 * @return 0 on sucessfull, others on failed.
 */
int rmdir(const char *pathname)
{
	int result;

	result = dfs_file_unlink(pathname);
	if (result < 0)
	{
		rt_set_errno(result);
		return -1;
	}

	return 0;
}

/**
 * this function is a POSIX compliant version, which will open a directory.
 *
 * @param name the path name to be open.
 *
 * @return the DIR pointer of directory, NULL on open failed.
 */
DIR *opendir(const char *name)
{
	struct dfs_fd *d;
	int fd, result;
	DIR *t;

	t = RT_NULL;

	/* allocate a fd */
	fd = fd_new();
	if (fd == -1)
	{
		rt_kprintf("no fd\n");
		return RT_NULL;
	}
	d = fd_get(fd);

	result = dfs_file_open(d, name, DFS_O_RDONLY | DFS_O_DIRECTORY);
	if (result >= 0)
	{
		/* open successfully */
		t = (DIR *) rt_malloc(sizeof(DIR));
		if (t == RT_NULL)
		{
			dfs_file_close(d);
			fd_put(d);
		}
		else
		{
		    rt_memset(t, 0, sizeof(DIR));
		    t->fd = fd;
		}
		fd_put(d);
		return t;
	}

	/* open failed */
	fd_put(d);
	fd_put(d);
	rt_set_errno(result);

	return RT_NULL;
}

/**
 * this function is a POSIX compliant version, which will return a pointer 
 * to a dirent structure representing the next directory entry in the 
 * directory stream.
 *
 * @param d the directory stream pointer.
 *
 * @return the next directory entry, NULL on the end of directory or failed.
 */
struct dirent *readdir(DIR *d)
{
	int result;
	struct dfs_fd *fd;

	fd = fd_get(d->fd);
	if (fd == RT_NULL)
	{
		rt_set_errno(-RT_ERROR);
		return RT_NULL;
	}

	if (!d->num || (d->cur += ((struct dirent*)(d->buf + d->cur))->d_reclen) >= d->num)
	{
		/* get a new entry */
		result = dfs_file_getdents(fd, (struct dirent*)d->buf, sizeof(d->buf) - 1);
		if (result <= 0)
		{
			fd_put(fd);
			rt_set_errno(result);

			return RT_NULL;
		}

		d->num = result;
		d->cur = 0; /* current entry index */
	}

	fd_put(fd);
	return (struct dirent *)(d->buf+d->cur);
}

/**
 * this function is a POSIX compliant version, which will return current 
 * location in directory stream.
 * 
 * @param d the directory stream pointer.
 *
 * @return the current location in directory stream.
 */
long telldir(DIR *d)
{
	struct dfs_fd *fd;
	long result;

	fd = fd_get(d->fd);
	if (fd == RT_NULL)
	{
		rt_set_errno(-RT_ERROR);
		return 0;
	}

	result = fd->pos - d->num + d->cur;
	fd_put(fd);

	return result;
}

/**
 * this function is a POSIX compliant version, which will set position of 
 * next directory structure in the directory stream.
 *
 * @param d the directory stream.
 * @param offset the offset in directory stream.
 */
void seekdir(DIR *d, off_t offset)
{
	struct dfs_fd *fd;

	fd = fd_get(d->fd);
	if (fd == RT_NULL)
	{
		rt_set_errno(-RT_ERROR);
		return ;
	}

	/* seek to the offset position of directory */
	if (dfs_file_lseek(fd, offset) >= 0)
		d->num = d->cur = 0;
	fd_put(fd);
}

/**
 * this function is a POSIX compliant version, which will reset directory stream.
 *
 * @param d the directory stream.
 */
void rewinddir(DIR *d)
{
	struct dfs_fd *fd;

	fd = fd_get(d->fd);
	if (fd == RT_NULL)
	{
		rt_set_errno(-RT_ERROR);
		return ;
	}

	/* seek to the beginning of directory */
	if (dfs_file_lseek(fd, 0) >= 0)
		d->num = d->cur = 0;
	fd_put(fd);
}

/**
 * this function is a POSIX compliant version, which will close a directory 
 * stream.
 * 
 * @param d the directory stream.
 *
 * @return 0 on successful, -1 on failed.
 */
int closedir(DIR *d)
{
	int result;
	struct dfs_fd *fd;

	fd = fd_get(d->fd);
	if (fd == RT_NULL)
	{
		rt_set_errno(-RT_ERROR);
		return -1;
	}

	result = dfs_file_close(fd);
	fd_put(fd);

	fd_put(fd);
	rt_free(d);

	if (result < 0)
	{
		rt_set_errno(result);
		return -1;
	}
	else return 0;
}

#ifdef DFS_USING_WORKDIR
/**
 * this function is a POSIX compliant version, which will change working directory.
 * 
 * @param path the path name to be changed to.
 *
 * @return 0 on successful, -1 on failed.
 */
int chdir(const char *path)
{
	char *fullpath;
	DIR *d;

	if (path == RT_NULL)
	{
		dfs_lock();
		rt_kprintf("%s\n", working_directory);
		dfs_unlock();
		return 0;
	}

	if (rt_strlen(path) > DFS_PATH_MAX)
		return -1;

	fullpath = dfs_normalize_path(NULL, path);
	if (fullpath == RT_NULL)
		return -1; /* build path failed */

	dfs_lock();
	d = opendir(fullpath);
	if (d == RT_NULL)
	{
		rt_free(fullpath);
		/* this is a not exist directory */
		dfs_unlock();
		return -1;
	}

	/* close directory stream */
	closedir(d);

	/* copy full path to working directory */
	strncpy(working_directory, fullpath, DFS_PATH_MAX);
	rt_free(fullpath); /* release normalize directory path name */

	dfs_unlock();

	return 0;
}
#ifdef RT_USING_FINSH
FINSH_FUNCTION_EXPORT_ALIAS(chdir, cd, change current working directory);
#endif
#endif

/**
 * this function is a POSIX compliant version, which will return current 
 * working directory.
 *
 * @param buf the returned current directory.
 * @param size the buffer size.
 *
 * @return the returned current directory.
 */
char *getcwd(char *buf, size_t size)
{
#ifdef DFS_USING_WORKDIR
	rt_enter_critical();
	rt_strncpy(buf, working_directory, size);
	rt_exit_critical();
#else
	rt_kprintf("WARNING: not support working directory\n");
#endif
	return buf;
}

/* @} */
