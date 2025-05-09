////////////////
// control_state_act.cc
// by Huang Yiting
////////////////
////////////////
act_str::act_str()
{
	assert(false);
}
//
act_str::act_str(int &order_stat)
{
	p_order_s = &order_stat;
}
//
std::string act_str::Idle()
{
	if (*p_order_s & ORDER_ST_GUARD) return Guard();
	if (*p_order_s & ORDER_ST_ENGAGE) return "Engage";
	return "Idle";
}
//
std::string act_str::Run()
{
	if (*p_order_s & ORDER_ST_WALK) return "Walk";
	return "Run";
}
//
std::string act_str::Jump()
{
	return "Jump";
}
//
std::string act_str::JumpLand()
{
	return "JumpLand";
}
//
std::string act_str::Engage()
{
	return "Engage";
}
//
std::string act_str::Damage()
{
	if (*p_order_s & ORDER_ST_GUARD) return "GuardDamage";
	return "Damage";
}
//
std::string act_str::Damage2()
{
	if (*p_order_s & ORDER_ST_GUARD) return "GuardDamage";
	return "Damage2";
}
//
std::string act_str::DamageStr1()
{
	return "Damage";
}
//
std::string act_str::DownCollapse()
{
	return "DownCollapse";
}
//
std::string act_str::Roll()
{
	return "Roll";
}
//
std::string act_str::WalkRev()
{
	return "WalkRev";
}
//
std::string act_str::Guard()
{
	return "Guard";
}
//
std::string act_str::DownKeep()
{
	return "DownKeep";
}
//
std::string act_str::GetUp()
{
	return "GetUp";
}
//
