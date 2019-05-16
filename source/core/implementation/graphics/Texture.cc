#include "../../include/graphics/lib.hh"



namespace mod {
  Texture::Texture (
    char const* in_origin,
    FIBITMAP* image,
    u8_t h_wrap,
    u8_t v_wrap,
    u8_t min_filter,
    u8_t mag_filter
  )
  : origin(str_clone(in_origin))
  {
    glGenTextures(1, &gl_id);
    glBindTexture(GL_TEXTURE_2D, gl_id);
    

    asset_assert_terminal(
      TextureWrap::validate(h_wrap) && TextureWrap::validate(v_wrap),
      "Cannot create new Texture with invalid wrap parameter(s) h: % " PRIu8 " v: %" PRIu8,
      h_wrap, v_wrap
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, TextureWrap::to_gl(h_wrap));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, TextureWrap::to_gl(v_wrap));


    asset_assert_terminal(
      TextureFilter::validate(min_filter) && TextureFilter::validate_mag(mag_filter),
      "Cannot create new Texture with invalid filter parameter(s) h: % " PRIu8 " v: %" PRIu8,
      min_filter, mag_filter
    );

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, TextureFilter::to_gl(min_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, TextureFilter::to_gl(mag_filter));


    glTexImage2D(
      GL_TEXTURE_2D,
      0,
      GL_RGBA8,
      FreeImage_GetWidth(image),
      FreeImage_GetHeight(image),
      0,
      GL_BGRA,
      GL_UNSIGNED_BYTE,
      FreeImage_GetBits(image)
    );

    if (TextureFilter::uses_mipmap(min_filter)) glGenerateMipmap(GL_TEXTURE_2D);


    glBindTexture(GL_TEXTURE_2D, 0);
  }


  Texture Texture::from_json_item (char const* origin, JSONItem const& json) {
    JSONItem* image_path_item = json.get_object_item("image_path");

    json.asset_assert(image_path_item != NULL, "Expected a String with key \"image_path\"");

    String& image_path = json.get_object_string("image_path");

    char relative_path [1024];

    image_path_item->asset_assert(
      str_dir_relativize_path(origin, image_path.value, relative_path, 1023),
      "Problem with JSON Texture image_path: The path given '%s' cannot be properly relativized to the asset path\n"
      "The combined path is either too long, or the asset path is not deep enough to accomodate the back tracking present in the relative image path",
      image_path.value
    );


    JSONItem* wrap_item = json.get_object_item("wrap");

    u8_t h_wrap;
    u8_t v_wrap;

    if (wrap_item != NULL) {
      String* h_wrap_s;
      String* v_wrap_s;

      if (wrap_item->type == JSONType::Array) {
        h_wrap_s = &wrap_item->get_array_string(0);
        v_wrap_s = &wrap_item->get_array_string(1);
      } else {
        h_wrap_s = v_wrap_s = &wrap_item->get_string();
      }
      
      h_wrap = TextureWrap::from_name(h_wrap_s->value);
      v_wrap = TextureWrap::from_name(v_wrap_s->value);

      wrap_item->get_array_item(0)->asset_assert(
        TextureWrap::validate(h_wrap),
        "Invalid wrap parameter for h_wrap '%s'. Valid values are:\n%s",
        h_wrap_s->value, TextureWrap::valid_values
      );

      wrap_item->get_array_item(1)->asset_assert(
        TextureWrap::validate(v_wrap),
        "Invalid wrap parameter for v_wrap '%s'. Valid values are:\n%s",
        v_wrap_s->value, TextureWrap::valid_values
      );
    } else {
      h_wrap = v_wrap = TextureWrap::Repeat;
    }

    
    JSONItem* filter_item = json.get_object_item("filter");

    u8_t min_filter;
    u8_t mag_filter;

    if (filter_item != NULL) {
      String* min_filter_s;
      String* mag_filter_s;

      if (filter_item->type == JSONType::Array) {
        min_filter_s = &filter_item->get_array_string(0);
        mag_filter_s = &filter_item->get_array_string(1);
      } else {
        min_filter_s = mag_filter_s = &filter_item->get_string();
      }
      
      min_filter = TextureFilter::from_name(min_filter_s->value);
      mag_filter = TextureFilter::from_name(mag_filter_s->value);

      filter_item->get_array_item(0)->asset_assert(
        TextureFilter::validate(min_filter),
        "Invalid filter parameter for min_filter '%s'. Valid min filters are:\n%s",
        min_filter_s->value, TextureFilter::valid_min_values
      );
      
      filter_item->get_array_item(1)->asset_assert(
        TextureFilter::validate_mag(mag_filter),
        "Invalid filter parameter for mag_filter '%s'. Valid mag filters are:\n%s",
        mag_filter_s->value, TextureFilter::valid_mag_values
      );
    } else {
      min_filter = mag_filter = TextureFilter::Nearest;
    }


    FREE_IMAGE_FORMAT format = FreeImage_GetFileType(relative_path);

    if (format == FIF_UNKNOWN) {
      char const* ext = NULL;

      s64_t ext_offset = str_file_extension(relative_path);

      if (ext_offset != -1) ext = relative_path + ext_offset;

      image_path_item->asset_error(
        "Cannot determine the file type of the image (Extension %s)\n"
        "Known file types are:\n"
        "FIF_BMP - Windows or OS/2 Bitmap File (*.BMP)\n"
        "FIF_ICO - Windows Icon (*.ICO)\n"
        "FIF_JPEG - Independent JPEG Group (*.JPG, *.JIF, *.JPEG, *.JPE)\n"
        "FIF_JNG - JPEG Network Graphics (*.JNG)\n"
        "FIF_KOALA - Commodore 64 Koala format (*.KOA)\n"
        "FIF_LBM - Amiga IFF (*.IFF, *.LBM)\n"
        "FIF_IFF - Amiga IFF (*.IFF, *.LBM)\n"
        "FIF_MNG - Multiple Network Graphics (*.MNG)\n"
        "FIF_PBM - Portable Bitmap (ASCII) (*.PBM)\n"
        "FIF_PBMRAW - Portable Bitmap (BINARY) (*.PBM)\n"
        "FIF_PCD - Kodak PhotoCD (*.PCD)\n"
        "FIF_PCX - Zsoft Paintbrush PCX bitmap format (*.PCX)\n"
        "FIF_PGM - Portable Graymap (ASCII) (*.PGM)\n"
        "FIF_PGMRAW - Portable Graymap (BINARY) (*.PGM)\n"
        "FIF_PNG - Portable Network Graphics (*.PNG)\n"
        "FIF_PPM - Portable Pixelmap (ASCII) (*.PPM)\n"
        "FIF_PPMRAW - Portable Pixelmap (BINARY) (*.PPM)\n"
        "FIF_RAS - Sun Rasterfile (*.RAS)\n"
        "FIF_TARGA - truevision Targa files (*.TGA, *.TARGA)\n"
        "FIF_TIFF - Tagged Image File Format (*.TIF, *.TIFF)\n"
        "FIF_WBMP - Wireless Bitmap (*.WBMP)\n"
        "FIF_PSD - Adobe Photoshop (*.PSD)\n"
        "FIF_CUT - Dr. Halo (*.CUT)\n"
        "FIF_XBM - X11 Bitmap Format (*.XBM)\n"
        "FIF_XPM - X11 Pixmap Format (*.XPM)\n"
        "FIF_DDS - DirectDraw Surface (*.DDS)\n"
        "FIF_GIF - Graphics Interchange Format (*.GIF)\n"
        "FIF_HDR - High Dynamic Range (*.HDR)\n"
        "FIF_FAXG3 - Raw Fax format CCITT G3 (*.G3)\n"
        "FIF_SGI - Silicon Graphics SGI image format (*.SGI)\n"
        "FIF_EXR - OpenEXR format (*.EXR)\n"
        "FIF_J2K - JPEG-2000 format (*.J2K, *.J2C)\n"
        "FIF_JP2 - JPEG-2000 format (*.JP2)\n"
        "FIF_PFM - Portable FloatMap (*.PFM)\n"
        "FIF_PICT - Macintosh PICT (*.PICT)\n"
        "FIF_RAW - RAW camera image (*.*)\n",
        ext
      );
    }

    FIBITMAP* image = FreeImage_Load(format, relative_path);
    
    m_asset_assert(image != NULL, relative_path, "Failed to load image");

    FIBITMAP* image32 = FreeImage_ConvertTo32Bits(image);

    FreeImage_Unload(image);

    m_asset_assert(image32 != NULL, relative_path, "Failed to convert image to 32 bpp");


    Texture texture;

    try {
     texture = { origin, image32, h_wrap, v_wrap, min_filter, mag_filter };
    } catch (Exception& exception) {
      FreeImage_Unload(image32);
      throw exception;
    }

    FreeImage_Unload(image32);

    return texture;
  }

  Texture Texture::from_str (char const* origin, char const* source) {
    JSON json = JSON::from_str(origin, source);

    Texture texture;

    try {
      texture = from_json(origin, json);
    } catch (Exception& exception) {
      json.destroy();
      throw exception;
    }

    json.destroy();

    return texture;
  }

  Texture Texture::from_file (char const* origin) {
    auto [ source, length ] = load_file(origin);

    m_asset_assert(
      source != NULL,
      origin,
      "Failed to load Texture: Unable to read file"
    );

    Texture texture;

    try {
      texture = from_str(origin, static_cast<char*>(source));
    } catch (Exception& exception) {
      memory::deallocate(source);
      throw exception;
    }

    memory::deallocate(source);

    return texture;
  }


  void Texture::destroy () {
    if (origin != NULL) memory::deallocate(origin);

    if (gl_id != 0) {
      glDeleteTextures(1, &gl_id);
    }
  }


  void Texture::update (FIBITMAP* new_image, char const* new_origin) {
    if (new_origin != NULL) {
      memory::deallocate(origin);
      origin = str_clone(new_origin);
    }

    glTextureSubImage2D(
      gl_id,
      0, 0, 0,
      FreeImage_GetWidth(new_image),
      FreeImage_GetHeight(new_image),
      GL_BGRA, GL_UNSIGNED_BYTE,
      FreeImage_GetBits(new_image)
    );

    if (TextureFilter::uses_mipmap(TextureFilter::from_gl(get_parameter<s32_t>(GL_TEXTURE_MIN_FILTER)))) {
      glGenerateTextureMipmap(gl_id);
    }
  }


  FIBITMAP* Texture::read (s32_t level) const {
    static Array<u8_t> data = Array<u8_t> { 0, true };
    
    Vector2s size = get_size();

    size_t mem_size = size.x * size.y * 4;

    data.reallocate(mem_size);

    glGetTextureImage(gl_id, level, GL_BGRA, GL_UNSIGNED_BYTE, mem_size, data.elements);

    FIBITMAP* image = FreeImage_ConvertFromRawBits(
      data.elements,
      size.x, size.y, size.x * 4, 32,
      FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK
    );

    asset_assert(image != NULL, "Unable to create image from raw pixel data for Texture::read");

    return image;
  }


  void Texture::set_wrap (u8_t h_wrap, u8_t v_wrap) const {
    asset_assert(
      TextureWrap::validate(h_wrap) && TextureWrap::validate(v_wrap),
      "Cannot set invalid wrap parameter(s) h: % " PRIu8 " v: %" PRIu8,
      h_wrap, v_wrap
    );

    set_parameter(GL_TEXTURE_WRAP_S, TextureWrap::to_gl(h_wrap));
    set_parameter(GL_TEXTURE_WRAP_T, TextureWrap::to_gl(v_wrap));
  }
  
  void Texture::set_filter (u8_t min_filter, u8_t mag_filter) const {
    asset_assert(
      TextureFilter::validate(min_filter) && TextureFilter::validate_mag(mag_filter),
      "Cannot set invalid filter parameter(s) min: % " PRIu8 " mag: %" PRIu8,
      min_filter, mag_filter
    );

    s32_t prev_min = get_parameter<s32_t>(GL_TEXTURE_MIN_FILTER);

    set_parameter(GL_TEXTURE_MIN_FILTER, TextureFilter::to_gl(min_filter));
    set_parameter(GL_TEXTURE_MAG_FILTER, TextureFilter::to_gl(mag_filter));

    if (TextureFilter::uses_mipmap(min_filter) && !TextureFilter::uses_mipmap(TextureFilter::from_gl(prev_min))) {
      glGenerateTextureMipmap(gl_id);
    }
  }
}