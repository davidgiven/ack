double TIME()
{

struct tbuffer { long proc_user_time ; long proc_system_time ; long child_user_time ; long child_system_time ; } ;
struct tbuffer tb ;

times( &tb ) ;
return( (double)( tb.proc_user_time + tb.proc_system_time + tb.child_user_time + tb.child_system_time ) / 60 ) ;

}
