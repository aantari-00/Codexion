#include "codexion.h"

int	can_take_dongles(t_coder *coder, long now)
{
	t_dongle	*left;
	t_dongle	*right;

	if (coder->left_dongle == coder->right_dongle)
		return (0);
	left = &coder->sim->dongles[coder->left_dongle];
	right = &coder->sim->dongles[coder->right_dongle];
	if (left->holder != -1 || now < left->available_time)
		return (0);
	if (right->holder != -1 || now < right->available_time)
		return (0);
	return (1);
}

void	take_dongles(t_coder *coder)
{
	coder->sim->dongles[coder->left_dongle].holder = coder->id;
	coder->sim->dongles[coder->right_dongle].holder = coder->id;
}

void	release_dongles(t_coder *coder, long now)
{
	t_dongle	*left;
	t_dongle	*right;

	left = &coder->sim->dongles[coder->left_dongle];
	right = &coder->sim->dongles[coder->right_dongle];
	left->holder = -1;
	left->available_time = now + coder->sim->args.dongle_cooldown;
	right->holder = -1;
	right->available_time = now + coder->sim->args.dongle_cooldown;
}
