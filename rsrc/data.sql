--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

SET search_path = public, pg_catalog;

--
-- Data for Name: auth_group; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY auth_group (id, name) FROM stdin;
\.


--
-- Name: auth_group_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('auth_group_id_seq', 1, false);


--
-- Data for Name: django_content_type; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY django_content_type (id, name, app_label, model) FROM stdin;
1	permission	auth	permission
2	group	auth	group
3	user	auth	user
4	message	auth	message
5	content type	contenttypes	contenttype
6	session	sessions	session
7	site	sites	site
8	log entry	admin	logentry
9	migration history	south	migrationhistory
10	blast	bugle	blast
11	image upload	bugle	imageupload
12	twitter profile	twitter_api	twitterprofile
\.


--
-- Data for Name: auth_permission; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY auth_permission (id, name, content_type_id, codename) FROM stdin;
1	Can add permission	1	add_permission
2	Can change permission	1	change_permission
3	Can delete permission	1	delete_permission
4	Can add group	2	add_group
5	Can change group	2	change_group
6	Can delete group	2	delete_group
7	Can add user	3	add_user
8	Can change user	3	change_user
9	Can delete user	3	delete_user
10	Can add message	4	add_message
11	Can change message	4	change_message
12	Can delete message	4	delete_message
13	Can add content type	5	add_contenttype
14	Can change content type	5	change_contenttype
15	Can delete content type	5	delete_contenttype
16	Can add session	6	add_session
17	Can change session	6	change_session
18	Can delete session	6	delete_session
19	Can add site	7	add_site
20	Can change site	7	change_site
21	Can delete site	7	delete_site
22	Can add log entry	8	add_logentry
23	Can change log entry	8	change_logentry
24	Can delete log entry	8	delete_logentry
25	Can add migration history	9	add_migrationhistory
26	Can change migration history	9	change_migrationhistory
27	Can delete migration history	9	delete_migrationhistory
28	Can add blast	10	add_blast
29	Can change blast	10	change_blast
30	Can delete blast	10	delete_blast
31	Can add image upload	11	add_imageupload
32	Can change image upload	11	change_imageupload
33	Can delete image upload	11	delete_imageupload
34	Can add twitter profile	12	add_twitterprofile
35	Can change twitter profile	12	change_twitterprofile
36	Can delete twitter profile	12	delete_twitterprofile
\.


--
-- Data for Name: auth_group_permissions; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY auth_group_permissions (id, group_id, permission_id) FROM stdin;
\.


--
-- Name: auth_group_permissions_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('auth_group_permissions_id_seq', 1, false);


--
-- Data for Name: auth_user; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY auth_user (id, username, first_name, last_name, email, password, is_staff, is_active, is_superuser, last_login, date_joined) FROM stdin;
1	subversion				sha1$6475f$2f31c70e45bb0c1131feac94e89a82b419b6e3c4	f	t	f	2015-05-23 22:25:56.267692+01	2015-05-23 22:25:56.267735+01
2	alexs			alexs@example.com	sha1$fc9a8$d5300b49426086a7eaf0c1eeea812c9fa397cbd2	f	t	f	2015-05-23 22:26:16.023293+01	2015-05-23 22:26:15.941677+01
3	ashb			ashb@example.com	sha1$16b4e$bb4f11eaddf5fc9c3b4ada311c53da38450aabb9	f	t	f	2015-05-23 22:26:17.449179+01	2015-05-23 22:26:17.41201+01
4	steve			steve@example.com	sha1$e0844$310781f1904d6675ccb39f3c22e03901d3344cae	f	t	f	2015-05-23 22:28:48.525925+01	2015-05-23 22:28:48.494588+01
6	DRMacIver			DRMacIver@example.com	sha1$b2427$b077b32f247bb45c3e586b0b35837d3a1a4482fc	f	t	f	2015-05-24 12:08:43.341397+01	2015-05-24 12:08:43.317275+01
7	Anna			Anna@example.com	sha1$84167$821b83e5fba06eb30c7c3038ba051a490a349801	f	t	f	2015-05-24 12:13:40.796338+01	2015-05-24 12:13:40.76165+01
8	pkqk			pkqk@example.com	sha1$e1c09$3379edd11e246ae5b29975ed72a18a6afebcb01b	f	t	f	2015-05-24 13:42:12.067789+01	2015-05-24 13:42:12.035845+01
5	nickstenning			nickstenning@example.com	sha1$df8c1$644147e7c52eebd7de94ad06b13eb59740a917a8	f	t	f	2015-05-24 12:08:43.108081+01	2015-05-24 12:08:43.039785+01
\.


--
-- Data for Name: auth_message; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY auth_message (id, user_id, message) FROM stdin;
\.


--
-- Name: auth_message_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('auth_message_id_seq', 1, false);


--
-- Name: auth_permission_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('auth_permission_id_seq', 36, true);


--
-- Data for Name: auth_user_groups; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY auth_user_groups (id, user_id, group_id) FROM stdin;
\.


--
-- Name: auth_user_groups_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('auth_user_groups_id_seq', 1, false);


--
-- Name: auth_user_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('auth_user_id_seq', 8, true);


--
-- Data for Name: auth_user_user_permissions; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY auth_user_user_permissions (id, user_id, permission_id) FROM stdin;
\.


--
-- Name: auth_user_user_permissions_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('auth_user_user_permissions_id_seq', 1, false);


--
-- Data for Name: bugle_blast; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY bugle_blast (id, user_id, message, created, extended, attachment, short, is_todo, is_broadcast, done, in_reply_to_id) FROM stdin;
1	2	Fort.	2015-05-23 22:26:28.343858+01			\N	f	f	f	\N
2	3	OH: "We're going to need a bigger moat."	2015-05-23 22:26:33.632764+01			\N	f	f	f	\N
3	5	"Yeah, because if you just `pushd; git pull; render; popd`, it would be..."	2015-05-24 12:09:18.829385+01			\N	f	f	f	\N
4	7	OH: "Basically, Govias"	2015-05-24 12:14:01.497509+01			\N	f	f	f	\N
5	6	OH: Ctrl + alt + Stevie Wonder	2015-05-24 13:31:56.84811+01			\N	f	f	f	\N
6	4	is having postgres read out to him from the TV. Exciting?	2015-05-24 13:40:37.817394+01			\N	f	f	f	\N
7	5	OH: "He's just doing chef. Someone has to, otherwise the universe ends."	2015-05-24 13:52:34.582085+01			\N	f	f	f	\N
8	7	Be very afraid	2015-05-24 15:19:42.622402+01			\N	f	f	f	\N
9	4	@nickstenning Postgres!	2015-05-24 15:24:14.137651+01		attachments/2015/05/24/postgresql-9.4.1.yosemite.bottle.1.tar.gz	\N	f	f	f	\N
10	4	@nickstenning More postgres!	2015-05-24 15:25:10.320947+01		attachments/2015/05/24/postgresql-9.4.1_1.yosemite.bottle.tar.gz	\N	f	f	f	\N
11	3	@anna	2015-05-24 15:53:12.081534+01	deb http://apt.fort/ trusty main universe restricted multiverse\r\ndeb http://apt.fort/ trusty-backports main universe restricted multiverse\r\ndeb http://apt.fort/ trusty-proposed main universe restricted multiverse\r\ndeb http://apt.fort/ trusty-security main universe restricted multiverse\r\ndeb http://apt.fort/ trusty-updates main universe restricted multiverse\r\n\r\ndeb-src http://apt.fort/ trusty main universe restricted multiverse\r\ndeb-src http://apt.fort/ trusty-backports main universe restricted multiverse\r\ndeb-src http://apt.fort/ trusty-proposed main universe restricted multiverse\r\ndeb-src http://apt.fort/ trusty-security main universe restricted multiverse\r\ndeb-src http://apt.fort/ trusty-updates main universe restricted multiverse		\N	f	f	f	\N
12	3	@Nickstenning makefile with gcc auto deps	2015-05-24 15:54:03.950045+01	OUTPUT=../bin/cms_backend\r\nIDIRS+= -I../common -I/usr/include/pcl-1.5 -I/usr/include/eigen3\r\nCFLAGS=-W -Wall \r\nCC=g++\r\nDEPS?=.deps\r\n\r\nCFLAGS += -DDEBUG -g \r\nLIBS=-lcommon -lTML_lib -ltmlcomm \r\n\r\nLFLAGS= ${LIBS} -L../lib -lz\r\nLFLAGS+=$(shell xml2-config --libs)\r\nCFLAGS+=$(shell xml2-config --cflags)\r\n\r\nSRCS=$(wildcard *.cpp  )\r\nOBJS=$(addsuffix .o, $(basename $(SRCS) ))\r\n\r\nall: ${OUTPUT}\r\n-include .deps/*.d\r\n\r\n\r\nclean:\r\n\trm -rf ${OUTPUT} *.o ${DEPS}\r\n\r\n# Compile a .o from a similarly named .cpp file, and re-gen the .dep file\r\n%.o: %.cpp\r\n\t@mkdir -p ${DEPS}\r\n\t@echo " CC   $(<:.cpp=.o)"\r\n\t@${CC} -MMD -MF ${DEPS}/$(notdir $(<:%.cpp=%.d) ) ${CFLAGS} ${IDIRS} -o $(<:.cpp=.o) -c $<\r\n\r\n\r\n../bin/cms_backend: ${OBJS}\r\n\t@echo " LD   $@"\r\n\t@${CC} ${CFLAGS} -o $@ ${OBJS} ${LFLAGS}\r\n\r\n\r\n		\N	f	f	f	\N
13	5	@ashb Check sources for /Library/Caches/Homebrew	2015-05-24 16:01:17.352971+01		attachments/2015/05/24/check-0.9.14.tar.gz	\N	f	f	f	\N
14	6	@ashb @anna http://docs.fort/check.sourceforge.net/doc/check_html/ for check docs	2015-05-24 16:32:19.197311+01			\N	f	f	f	\N
15	4	“COLOR_YELLOW is actually brown.” — `man curs_color`, on the subject of yellow.	2015-05-24 16:40:29.702603+01			\N	f	f	f	\N
16	5	pkg-config file for libpq	2015-05-24 17:05:03.096555+01	Name: libpq\r\nDescription: PostgreSQL libpq library\r\nUrl: http://www.postgresql.org/\r\nVersion: 9.4.1\r\nRequires:\r\nRequires.private:\r\nCflags: -I/usr/local/Cellar/postgresql/9.4.1_1/include\r\nLibs: -L/usr/local/Cellar/postgresql/9.4.1_1/lib -lpq\r\nLibs.private:  -lssl -lcrypto -lgssapi_krb5 -lldap_r -lpthread		\N	f	f	f	\N
17	5	openssl 1.0.2a-1 for /Library/Caches/Homebrew	2015-05-24 17:06:56.927187+01		attachments/2015/05/24/openssl-1.0.2a-1.yosemite.bottle.tar.gz	\N	f	f	f	\N
18	3	@DRMacIver @nickstenning this might help	2015-05-24 17:11:28.674509+01	CFLAGS+=$(foreach pkg,$(LIBS),$(shell pkg-config --silence-errors --cflags $(pkg)))\r\nLDFLAGS+=$(foreach pkg,$(LIBS),$(shell pkg-config --silence-errors --libs $(pkg)))\r\n		\N	f	f	f	\N
19	5	@pkqk openssl shlibs/binaries	2015-05-24 17:22:50.147896+01		attachments/2015/05/24/openssl-1.0.2a-1.tgz	\N	f	f	f	\N
20	5	@pkqk aaaand postgres	2015-05-24 17:24:29.875254+01		attachments/2015/05/24/postgresql-9.4.1-1.tar.gz	\N	f	f	f	\N
21	2	Vim.	2015-05-24 17:26:57.227752+01	syntax on\r\ncolorscheme pablo\r\nfiletype plugin indent on\r\n\r\nimap <C-a> <Esc>^I\r\nimap <C-e> <Esc>A\r\nimap <C-k> <Esc>d$A\r\nimap <C-y> <Esc>pA\r\n\r\nset modeline\r\nset autoindent\r\nset tabstop=4\r\nset expandtab\r\nset shiftwidth=4\r\n		\N	f	f	f	\N
22	8	👊	2015-05-24 20:24:39.628832+01			\N	f	f	f	\N
23	8	🇵🇷	2015-05-24 20:26:08.639747+01			\N	f	f	f	\N
24	8	👨🏽	2015-05-24 20:26:20.729233+01			\N	f	f	f	\N
25	3	a﻿b	2015-05-24 20:30:08.979336+01			\N	f	f	f	\N
26	2	Oh god it works.	2015-05-24 22:32:06.749734+01			\N	f	f	f	\N
27	2	Maybe?	2015-05-24 22:38:29.42242+01			\N	f	f	f	\N
28	2	!	2015-05-24 22:40:41.227749+01			\N	f	f	f	\N
29	2	!!	2015-05-24 22:41:44.867296+01			\N	f	f	f	\N
30	4	Live updates? Pah.	2015-05-24 22:41:59.615437+01			\N	f	f	f	\N
31	4	Hi @alexs!	2015-05-24 22:43:12.260487+01			\N	f	f	f	\N
32	3	@Steve run this please	2015-05-24 22:45:27.694977+01	cat > /usr/local/Library/Formula/ncurses.rb <EOF\r\nclass Ncurses < Formula\r\n  homepage ""\r\n  url "http://ncurses.org/ncurses-5.9.tar.gz"\r\n  version "5.9"\r\n  sha256 "20fe1b75a72244b2005e2393a4bc8a5e9725e7d242f322ea414d01dd6e37c947"\r\n\r\n  bottle do\r\n    sha256 "2af8bc1088c6d250a1f6816ebed58fda336ad3e90de7a56dd16d3c33584db416" => :yosemite\r\n  end\r\n\r\n  def install\r\n    #ENV.deparallelize  # if your formula fails when building in parallel\r\n\r\n    system "env"\r\n    system "./configure", #"--disable-debug",\r\n                          #"--disable-dependency-tracking",\r\n                          #"--disable-silent-rules",\r\n                          "--enable-pc-files",\r\n                          "--without-ada",\r\n                          "--without-tests",\r\n                          "--without-progs",\r\n                          "--with-chtype=long",\r\n                          "--with-mmask-t=long",\r\n                          "--with-shared",\r\n                          "--without-debug",\r\n                          "--enable-echo",\r\n                          "--enable-const",\r\n                          "--enable-symlinks",\r\n                          "--disable-lp64",\r\n                          "--enable-widec",\r\n                          "--prefix=#{prefix}"\r\n    system "make"\r\n    system "make", "install"\r\n  end\r\n\r\n  test do\r\n    # `test do` will create, run in and delete a temporary directory.\r\n    #\r\n    # This test will fail and we won't accept that! It's enough to just replace\r\n    # "false" with the main program this formula installs, but it'd be nice if you\r\n    # were more thorough. Run the test with `brew test ncurses`. Options passed\r\n    # to `brew install` such as `--HEAD` also need to be provided to `brew test`.\r\n    #\r\n    # The installed folder is not in the path, so use the entire path to any\r\n    # executables being tested: `system "#{bin}/program", "do", "something"`.\r\n    system "true"\r\n  end\r\nend\r\nEOF		\N	f	f	f	\N
33	3	@Steve and then here's the bottle	2015-05-24 22:45:59.151941+01		attachments/2015/05/24/ncurses-5.9.yosemite.bottle.tar.gz	\N	f	f	f	\N
34	4	🐚	2015-05-24 23:04:31.422265+01			\N	f	f	f	\N
35	2	Now with updates?	2015-05-24 23:06:30.822614+01			\N	f	f	f	\N
36	2	Updates!	2015-05-24 23:12:05.97525+01			\N	f	f	f	\N
37	4	Updates?	2015-05-24 23:17:53.969882+01			\N	f	f	f	\N
38	4	⬆️⬇️⬇️⬇️⬅️➡️⬅️➡️AB⏎	2015-05-24 23:18:44.705021+01			\N	f	f	f	\N
39	4	OK, fine @ash: ↑↑↓↓←→←→AB⏎	2015-05-24 23:20:13.539044+01			\N	f	f	f	\N
40	5	DANCER GHOST SMILING PILE OF POO	2015-05-24 23:31:20.919548+01			\N	f	f	f	\N
41	2	TERM=xterm-256color ./conch	2015-05-24 23:46:02.96601+01			\N	f	f	f	\N
42	4	@alex —	2015-05-24 23:55:11.004074+01			\N	f	f	f	\N
\.


--
-- Data for Name: bugle_blast_favourited_by; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY bugle_blast_favourited_by (id, blast_id, user_id) FROM stdin;
1	5	8
2	8	8
\.


--
-- Name: bugle_blast_favourited_by_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('bugle_blast_favourited_by_id_seq', 2, true);


--
-- Name: bugle_blast_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('bugle_blast_id_seq', 42, true);


--
-- Data for Name: bugle_blast_mentioned_users; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY bugle_blast_mentioned_users (id, blast_id, user_id) FROM stdin;
1	9	5
2	10	5
3	13	3
4	14	3
5	18	6
6	18	5
7	19	8
8	20	8
9	31	2
\.


--
-- Name: bugle_blast_mentioned_users_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('bugle_blast_mentioned_users_id_seq', 9, true);


--
-- Data for Name: bugle_imageupload; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY bugle_imageupload (id, user_id, attachment) FROM stdin;
\.


--
-- Name: bugle_imageupload_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('bugle_imageupload_id_seq', 1, false);


--
-- Data for Name: django_admin_log; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY django_admin_log (id, action_time, user_id, content_type_id, object_id, object_repr, action_flag, change_message) FROM stdin;
\.


--
-- Name: django_admin_log_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('django_admin_log_id_seq', 1, false);


--
-- Name: django_content_type_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('django_content_type_id_seq', 12, true);


--
-- Data for Name: django_session; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY django_session (session_key, session_data, expire_date) FROM stdin;
3627024510616732645528241be6c414	gAJ9cQEoVQp0ZXN0Y29va2llVQZ3b3JrZWRVEl9hdXRoX3VzZXJfYmFja2VuZHECVSlkamFuZ28u\nY29udHJpYi5hdXRoLmJhY2tlbmRzLk1vZGVsQmFja2VuZHEDVQ1fYXV0aF91c2VyX2lkcQRLAnUu\nMjRkMGFhYjQ0NjJlNDBjMzI3NGU5MmViNDA5MjliYzM=\n	2015-06-06 22:26:16.147189+01
82f39bc92fa397fef0840db1e31b5468	gAJ9cQEoVRJfYXV0aF91c2VyX2JhY2tlbmRxAlUpZGphbmdvLmNvbnRyaWIuYXV0aC5iYWNrZW5k\ncy5Nb2RlbEJhY2tlbmRxA1UNX2F1dGhfdXNlcl9pZHEESwN1LjA0NDBiYzBkNTk0NzYxMmJjYTk5\nNTA2NjQxNDJkNTRl\n	2015-06-06 22:26:17.530279+01
74e2f6181c6e42140a3ca0eeb55c0971	gAJ9cQEoVRJfYXV0aF91c2VyX2JhY2tlbmRxAlUpZGphbmdvLmNvbnRyaWIuYXV0aC5iYWNrZW5k\ncy5Nb2RlbEJhY2tlbmRxA1UNX2F1dGhfdXNlcl9pZHEESwR1LmM5NDE3M2I0NzFjZjNjZWE5ZWQ2\nYjIyODU2YzQxNWY2\n	2015-06-06 22:28:48.584607+01
f499a2b1ff908e8060dab857c73576a0	gAJ9cQEoVQp0ZXN0Y29va2llVQZ3b3JrZWRVEl9hdXRoX3VzZXJfYmFja2VuZHECVSlkamFuZ28u\nY29udHJpYi5hdXRoLmJhY2tlbmRzLk1vZGVsQmFja2VuZHEDVQ1fYXV0aF91c2VyX2lkcQRLBnUu\nYjhiZjg2YTFiYzY4NDMzM2U0MmRlNTQwMWU5NDIxMjk=\n	2015-06-07 12:08:43.417122+01
eff435d8f31e8ac9a8c5b9813c8b383d	gAJ9cQEoVQp0ZXN0Y29va2llVQZ3b3JrZWRVEl9hdXRoX3VzZXJfYmFja2VuZHECVSlkamFuZ28u\nY29udHJpYi5hdXRoLmJhY2tlbmRzLk1vZGVsQmFja2VuZHEDVQ1fYXV0aF91c2VyX2lkcQRLB3Uu\nNTMwYWJmZGIyMzJjZjgyNGI2ZTlkZmI0N2YzNzlmOTg=\n	2015-06-07 12:13:40.871955+01
67f8eb49ed24d34ad083e8591804cb9f	gAJ9cQEoVRJfYXV0aF91c2VyX2JhY2tlbmRxAlUpZGphbmdvLmNvbnRyaWIuYXV0aC5iYWNrZW5k\ncy5Nb2RlbEJhY2tlbmRxA1UNX2F1dGhfdXNlcl9pZHEESwh1LjIwNTAwZWViMjMxZjI2YmZlYmZi\nZjI0YzYyZjVkOTcy\n	2015-06-07 13:42:12.14894+01
a06bd794fc534ddd80dc3c4a39d49a32	gAJ9cQEoVQp0ZXN0Y29va2llcQJVBndvcmtlZHEDVRJfYXV0aF91c2VyX2JhY2tlbmRVKWRqYW5n\nby5jb250cmliLmF1dGguYmFja2VuZHMuTW9kZWxCYWNrZW5kVQ1fYXV0aF91c2VyX2lkSwV1LjNm\nNjEyZWZiNDEwMWYyMDljNzVmMjIyMjZmMjBkMGY5\n	2015-06-07 13:43:44.017127+01
78b2841259a0065e0d0ba58563683d0a	gAJ9cQFVCnRlc3Rjb29raWVxAlUGd29ya2VkcQNzLmM0OTlmYWZlNGVmNDYzZTdhOTE3NzMxMDJk\nYzE4NzNi\n	2015-06-07 14:08:42.373127+01
\.


--
-- Data for Name: django_site; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY django_site (id, domain, name) FROM stdin;
1	example.com	example.com
\.


--
-- Name: django_site_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('django_site_id_seq', 1, true);


--
-- Data for Name: south_migrationhistory; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY south_migrationhistory (id, app_name, migration, applied) FROM stdin;
1	bugle	0001_initial	2015-05-23 21:25:56.191444+01
2	bugle	0002_add_subversion_user	2015-05-23 21:25:56.287665+01
3	bugle	0003_auto__add_field_blast_in_reply_to	2015-05-23 21:25:56.454238+01
4	twitter_api	0001_initial	2015-05-23 21:25:56.979692+01
\.


--
-- Name: south_migrationhistory_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('south_migrationhistory_id_seq', 4, true);


--
-- Data for Name: twitter_api_twitterprofile; Type: TABLE DATA; Schema: public; Owner: bugle
--

COPY twitter_api_twitterprofile (id, user_id, profile_image) FROM stdin;
\.


--
-- Name: twitter_api_twitterprofile_id_seq; Type: SEQUENCE SET; Schema: public; Owner: bugle
--

SELECT pg_catalog.setval('twitter_api_twitterprofile_id_seq', 1, false);


--
-- PostgreSQL database dump complete
--

