/// @file render.c
/// @author namazso
/// @date 2017-10-11
/// @brief Renderer implementation.
///
/// Collects and renders the sprites onto the global bitrmap.

#include "pch.h"

#include "string.h"
#include "render.h"
#include "sprite_manager.h"
#include "game.h"
#include "tile_manager.h"

/// @addtogroup render
/// @{

/// Enumeration of renderable basic command types.
enum render_cmd_type
{
	RenderCmd_Sprite,
	RenderCmd_Tile,
	RenderCmd_String,
};


/// Head of a render command.
struct render_cmd_head
{
	/// Pointer to the next command. NULL if last.
	struct render_cmd_head* next;

	/// The type of the command.
	enum render_cmd_type type;
};

/// Command to render a sprite.
struct render_cmd_sprite
{
	/// Head of the command.
	struct render_cmd_head head;

	/// Vertical position.
	int x;

	/// Horizontal position.
	int y;

	/// ID of the sprite to draw.
	int sprite_id;
};

/// Command to render a tile.
struct render_cmd_tile
{
	/// Head of the command.
	struct render_cmd_head head;

	/// Vertical position.
	int x;

	/// Horizontal position.
	int y;

	/// ID of the tile to draw.
	int tile_id;
};

/// Command to render a string.
struct render_cmd_string
{
	/// Head of the command.
	struct render_cmd_head head;

	/// Vertical position.
	int x;

	/// Horizontal position.
	int y;

	/// Sprite ID for the \0 character of the font.
	///
	/// The next 127 sprites will be the sprites used for drawing the
	/// string.
	int font_sprite;

	/// Length of the string.
	int len;

	/// The string.
	char str[1];
};

/// Head of the linked list of render commands.
struct render_cmds
{
	/// First element, NULL if none.
	struct render_cmd_head* first;

	/// Last element. May be invalid if first is NULL.
	struct render_cmd_head* last;
} s_cmds;

/// Add a new render command.
///
/// Allocates and adds a new render command struct of the desired size,
/// and initializes it.
///
/// @param[in] type Type of the render command to create.
/// @param[in] size Size of the new render command.
/// @return Pointer to the newly created command.
static struct render_cmd_head* add_new_cmd(enum render_cmd_type type,
	int size)
{
	struct render_cmd_head* cmd = (struct render_cmd_head*)malloc(size);
	assert(cmd);
	if (!s_cmds.first)
	{
		s_cmds.first = cmd;
		s_cmds.last = cmd;
	}
	else
	{
		s_cmds.last->next = cmd;
		s_cmds.last = cmd;
	}
	cmd->next = NULL;
	cmd->type = type;
	return cmd;
}

/// Initialize the render manager
void render_init(void)
{
	s_cmds.first = NULL;
	s_cmds.last = NULL;
}

/// Start a new series of render commands.
void render_start_frame(void)
{
	struct render_cmd_head* it = s_cmds.first;
	while (it != NULL)
	{
		struct render_cmd_head* next = it->next;
		free(it);
		it = next;
	}
	s_cmds.first = NULL;
	s_cmds.last = NULL;
}

/// Draw a sprite.
///
/// @param[in] id Sprite ID to draw.
/// @param[in] x Vertical position.
/// @param[in] y Horizontal position.
void render_sprite(int id, int x, int y)
{
	struct render_cmd_sprite* cmd = (struct render_cmd_sprite*)
		add_new_cmd(RenderCmd_Sprite, sizeof(struct render_cmd_sprite));

	cmd->x = x;
	cmd->y = y;
	cmd->sprite_id = id;
}

/// Draw a tile.
///
/// @param[in] id Tile id to draw.
/// @param[in] x Vertical position.
/// @param[in] y Horizontal position.
void render_tile(int id, int x, int y)
{
	struct render_cmd_tile* cmd = (struct render_cmd_tile*)
		add_new_cmd(RenderCmd_Tile, sizeof(struct render_cmd_tile));

	cmd->x = x;
	cmd->y = y;
	cmd->tile_id = id;
}

/// Render text somewhere.
///
/// @param[in] font_spr ID of the first sprite of the font. A font
///                     consists of 128 consecutive sprites.
/// @param[in] x Vertical position.
/// @param[in] y Horizontal position.
/// @param[in] str String to print. Only ASCII supported.
void render_print(int font_spr, int x, int y, const char* str)
{
	const int size = (int)strlen(str) + 1;
	struct render_cmd_string* cmd = (struct render_cmd_string*)
		add_new_cmd(RenderCmd_String,
			sizeof(struct render_cmd_string) + size);

	cmd->x = x;
	cmd->y = y;
	cmd->font_sprite = font_spr;
	cmd->len = size - 1;
	strcpy_s(cmd->str, size, str);
}

/// Draw a sprite render command
/// @param[in] cmd The command
static void render_draw_sprite(const struct render_cmd_sprite* cmd)
{
	sprite_draw_on_bitmap(g_bitmap, k_pixel_width, k_pixel_height,
		sprite_manager_get_by_id(cmd->sprite_id), cmd->x, cmd->y);
}

/// Draw a tile render command
/// @param[in] cmd The command
static void render_draw_tile(const struct render_cmd_tile* cmd)
{
	tile_draw_on_bitmap(g_bitmap, k_pixel_width, k_pixel_height,
		tile_manager_get_by_id(cmd->tile_id), cmd->x, cmd->y);
}

/// Draw a string render command
/// @param[in] cmd The command
static void render_draw_string(const struct render_cmd_string* cmd)
{
	for (int i = 0; i < cmd->len; ++i)
	{
		const int spr = cmd->font_sprite + ((uint8_t*)cmd->str)[i];
		const int x = cmd->x + i * k_sprite_size;
		const int y = cmd->y;
		sprite_draw_on_bitmap(g_bitmap, k_pixel_width, k_pixel_height,
			sprite_manager_get_by_id(spr), x, y);
	}
}

/// Render the current draw commands onto the global bitmap.
void render_render(void)
{
	// Paint the background black
	for (int i = 0; i < k_pixel_width; ++i)
		for (int j = 0; j < k_pixel_height; ++j)
			g_bitmap[j * k_pixel_width + i] = color_rgba(0, 0, 0, 255);

	for (const struct render_cmd_head* it = s_cmds.first;
		it != NULL;
		it = it->next)
	{
		switch (it->type)
		{
		case RenderCmd_Sprite:
			render_draw_sprite((const struct render_cmd_sprite*)it);
			break;
		case RenderCmd_Tile:
			render_draw_tile((const struct render_cmd_tile*)it);
			break;
		case RenderCmd_String:
			render_draw_string((const struct render_cmd_string*)it);
			break;
		default:
			assert(false);
			break;
		}
	}
}

extern void render_printf(int font_spr, int x, int y,
	const char* fmt, ...)
{
	// +1 for odd widths and +1 for the null.
	char str[(k_pixel_width + 1) / k_sprite_size + 1];
	va_list args;
	va_start(args, fmt);
	vsprintf_s(str, sizeof(str), fmt, args);
	va_end(args);
	render_print(font_spr, x, y, str);
}

/// @}
