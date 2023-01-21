void mutex_lock(void **mutex)
{
	WaitForSingleObject(*mutex,INFINITE);
}
void mutex_unlock(void **mutex)
{
	ReleaseMutex(*mutex);
}