int old_terrain_adjacent (world_t *world_t, int x, int y, int terrain)
{
  int work,
      work2,
      work_x,
      work_y;

  hex_t *hex;

  for (work = 1, work2 = 0; work < 7; work ++)
  {
    move_coords (world_t, work, x, y, &work_x, &work_y);

    if (x != work_x || y != work_y)
      {
	hex = get_hex_ref(world_t, work_x, work_y);
	if (hex->terrain == terrain)
	  work2 ++;
      }
  }
  return (work2);
}



/* ----------------------------------------------------------------------
--
-- get_hex
--
---------------------------------------------------------------------- */
void old_get_hex(world_t *world_t, int x, int y, hex_t *hex)
{
  memcpy(hex, &world_t->hexes[x * world_t->y + y], sizeof(hex_t));
}

/* ----------------------------------------------------------------------
--
-- get_hex_ref
--
---------------------------------------------------------------------- */
hex_t* get_hex_ref(world_t *world_t, int x, int y)
{
  return &world_t->hexes[x * world_t->y + y];
}

void old_put_hex(world_t *world_t, int x, int y, hex_t *hex)
{
  memcpy(&world_t->hexes[x * world_t->y + y], hex, sizeof(hex_t));
}
