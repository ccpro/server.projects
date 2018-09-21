#ifndef __NEWS_H__
#define __NEWS_H__

class News 
{
   public:
      static const int TYPE_SITE_NEWS		= 0x0001;
      static const int TYPE_MUSIC_NEWS		= 0x0002;
      static const int TYPE_PHOTOS_NEWS		= 0x0004;
      static const int TYPE_BOOKS_NEWS		= 0x0080;
      static const int TYPE_NOTE_NEWS		= 0x0010;
      static const int TYPE_MOVIES_NEWS		= 0x0008;
      static const int TYPE_HTTP_LINK_NEWS	= 0x0020;
      static const int TYPE_ARTICLE_NEWS	= 0x0100;

      static int addNews(PgDatabase &pg, const int type, const string subject, const string news, const int owner, const int level);
};

#endif // __NEWS_H__
