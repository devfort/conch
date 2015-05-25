--
-- PostgreSQL database dump
--

SET statement_timeout = 0;
SET lock_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SET check_function_bodies = false;
SET client_min_messages = warning;

--
-- Name: plpgsql; Type: EXTENSION; Schema: -; Owner: 
--

CREATE EXTENSION IF NOT EXISTS plpgsql WITH SCHEMA pg_catalog;


--
-- Name: EXTENSION plpgsql; Type: COMMENT; Schema: -; Owner: 
--

COMMENT ON EXTENSION plpgsql IS 'PL/pgSQL procedural language';


SET search_path = public, pg_catalog;

SET default_tablespace = '';

SET default_with_oids = false;

--
-- Name: auth_group; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE auth_group (
    id integer NOT NULL,
    name character varying(80) NOT NULL
);


ALTER TABLE public.auth_group OWNER TO bugle;

--
-- Name: auth_group_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE auth_group_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.auth_group_id_seq OWNER TO bugle;

--
-- Name: auth_group_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE auth_group_id_seq OWNED BY auth_group.id;


--
-- Name: auth_group_permissions; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE auth_group_permissions (
    id integer NOT NULL,
    group_id integer NOT NULL,
    permission_id integer NOT NULL
);


ALTER TABLE public.auth_group_permissions OWNER TO bugle;

--
-- Name: auth_group_permissions_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE auth_group_permissions_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.auth_group_permissions_id_seq OWNER TO bugle;

--
-- Name: auth_group_permissions_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE auth_group_permissions_id_seq OWNED BY auth_group_permissions.id;


--
-- Name: auth_message; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE auth_message (
    id integer NOT NULL,
    user_id integer NOT NULL,
    message text NOT NULL
);


ALTER TABLE public.auth_message OWNER TO bugle;

--
-- Name: auth_message_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE auth_message_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.auth_message_id_seq OWNER TO bugle;

--
-- Name: auth_message_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE auth_message_id_seq OWNED BY auth_message.id;


--
-- Name: auth_permission; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE auth_permission (
    id integer NOT NULL,
    name character varying(50) NOT NULL,
    content_type_id integer NOT NULL,
    codename character varying(100) NOT NULL
);


ALTER TABLE public.auth_permission OWNER TO bugle;

--
-- Name: auth_permission_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE auth_permission_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.auth_permission_id_seq OWNER TO bugle;

--
-- Name: auth_permission_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE auth_permission_id_seq OWNED BY auth_permission.id;


--
-- Name: auth_user; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE auth_user (
    id integer NOT NULL,
    username character varying(30) NOT NULL,
    first_name character varying(30) NOT NULL,
    last_name character varying(30) NOT NULL,
    email character varying(75) NOT NULL,
    password character varying(128) NOT NULL,
    is_staff boolean NOT NULL,
    is_active boolean NOT NULL,
    is_superuser boolean NOT NULL,
    last_login timestamp with time zone NOT NULL,
    date_joined timestamp with time zone NOT NULL
);


ALTER TABLE public.auth_user OWNER TO bugle;

--
-- Name: auth_user_groups; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE auth_user_groups (
    id integer NOT NULL,
    user_id integer NOT NULL,
    group_id integer NOT NULL
);


ALTER TABLE public.auth_user_groups OWNER TO bugle;

--
-- Name: auth_user_groups_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE auth_user_groups_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.auth_user_groups_id_seq OWNER TO bugle;

--
-- Name: auth_user_groups_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE auth_user_groups_id_seq OWNED BY auth_user_groups.id;


--
-- Name: auth_user_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE auth_user_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.auth_user_id_seq OWNER TO bugle;

--
-- Name: auth_user_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE auth_user_id_seq OWNED BY auth_user.id;


--
-- Name: auth_user_user_permissions; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE auth_user_user_permissions (
    id integer NOT NULL,
    user_id integer NOT NULL,
    permission_id integer NOT NULL
);


ALTER TABLE public.auth_user_user_permissions OWNER TO bugle;

--
-- Name: auth_user_user_permissions_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE auth_user_user_permissions_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.auth_user_user_permissions_id_seq OWNER TO bugle;

--
-- Name: auth_user_user_permissions_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE auth_user_user_permissions_id_seq OWNED BY auth_user_user_permissions.id;


--
-- Name: bugle_blast; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE bugle_blast (
    id integer NOT NULL,
    user_id integer NOT NULL,
    message text NOT NULL,
    created timestamp with time zone DEFAULT '2015-05-23 22:25:55.431665+01'::timestamp with time zone NOT NULL,
    extended text,
    attachment character varying(100) NOT NULL,
    short character varying(50),
    is_todo boolean DEFAULT false NOT NULL,
    is_broadcast boolean DEFAULT false NOT NULL,
    done boolean DEFAULT false NOT NULL,
    in_reply_to_id integer
);


ALTER TABLE public.bugle_blast OWNER TO bugle;

--
-- Name: bugle_blast_favourited_by; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE bugle_blast_favourited_by (
    id integer NOT NULL,
    blast_id integer NOT NULL,
    user_id integer NOT NULL
);


ALTER TABLE public.bugle_blast_favourited_by OWNER TO bugle;

--
-- Name: bugle_blast_favourited_by_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE bugle_blast_favourited_by_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.bugle_blast_favourited_by_id_seq OWNER TO bugle;

--
-- Name: bugle_blast_favourited_by_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE bugle_blast_favourited_by_id_seq OWNED BY bugle_blast_favourited_by.id;


--
-- Name: bugle_blast_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE bugle_blast_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.bugle_blast_id_seq OWNER TO bugle;

--
-- Name: bugle_blast_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE bugle_blast_id_seq OWNED BY bugle_blast.id;


--
-- Name: bugle_blast_mentioned_users; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE bugle_blast_mentioned_users (
    id integer NOT NULL,
    blast_id integer NOT NULL,
    user_id integer NOT NULL
);


ALTER TABLE public.bugle_blast_mentioned_users OWNER TO bugle;

--
-- Name: bugle_blast_mentioned_users_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE bugle_blast_mentioned_users_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.bugle_blast_mentioned_users_id_seq OWNER TO bugle;

--
-- Name: bugle_blast_mentioned_users_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE bugle_blast_mentioned_users_id_seq OWNED BY bugle_blast_mentioned_users.id;


--
-- Name: bugle_imageupload; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE bugle_imageupload (
    id integer NOT NULL,
    user_id integer NOT NULL,
    attachment character varying(100) NOT NULL
);


ALTER TABLE public.bugle_imageupload OWNER TO bugle;

--
-- Name: bugle_imageupload_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE bugle_imageupload_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.bugle_imageupload_id_seq OWNER TO bugle;

--
-- Name: bugle_imageupload_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE bugle_imageupload_id_seq OWNED BY bugle_imageupload.id;


--
-- Name: conch_auth_user; Type: VIEW; Schema: public; Owner: postgres
--

CREATE VIEW conch_auth_user AS
 SELECT auth_user.id,
    auth_user.username
   FROM auth_user;


ALTER TABLE public.conch_auth_user OWNER TO postgres;

--
-- Name: django_admin_log; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE django_admin_log (
    id integer NOT NULL,
    action_time timestamp with time zone NOT NULL,
    user_id integer NOT NULL,
    content_type_id integer,
    object_id text,
    object_repr character varying(200) NOT NULL,
    action_flag smallint NOT NULL,
    change_message text NOT NULL,
    CONSTRAINT django_admin_log_action_flag_check CHECK ((action_flag >= 0))
);


ALTER TABLE public.django_admin_log OWNER TO bugle;

--
-- Name: django_admin_log_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE django_admin_log_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.django_admin_log_id_seq OWNER TO bugle;

--
-- Name: django_admin_log_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE django_admin_log_id_seq OWNED BY django_admin_log.id;


--
-- Name: django_content_type; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE django_content_type (
    id integer NOT NULL,
    name character varying(100) NOT NULL,
    app_label character varying(100) NOT NULL,
    model character varying(100) NOT NULL
);


ALTER TABLE public.django_content_type OWNER TO bugle;

--
-- Name: django_content_type_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE django_content_type_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.django_content_type_id_seq OWNER TO bugle;

--
-- Name: django_content_type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE django_content_type_id_seq OWNED BY django_content_type.id;


--
-- Name: django_session; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE django_session (
    session_key character varying(40) NOT NULL,
    session_data text NOT NULL,
    expire_date timestamp with time zone NOT NULL
);


ALTER TABLE public.django_session OWNER TO bugle;

--
-- Name: django_site; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE django_site (
    id integer NOT NULL,
    domain character varying(100) NOT NULL,
    name character varying(50) NOT NULL
);


ALTER TABLE public.django_site OWNER TO bugle;

--
-- Name: django_site_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE django_site_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.django_site_id_seq OWNER TO bugle;

--
-- Name: django_site_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE django_site_id_seq OWNED BY django_site.id;


--
-- Name: south_migrationhistory; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE south_migrationhistory (
    id integer NOT NULL,
    app_name character varying(255) NOT NULL,
    migration character varying(255) NOT NULL,
    applied timestamp with time zone NOT NULL
);


ALTER TABLE public.south_migrationhistory OWNER TO bugle;

--
-- Name: south_migrationhistory_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE south_migrationhistory_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.south_migrationhistory_id_seq OWNER TO bugle;

--
-- Name: south_migrationhistory_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE south_migrationhistory_id_seq OWNED BY south_migrationhistory.id;


--
-- Name: twitter_api_twitterprofile; Type: TABLE; Schema: public; Owner: bugle; Tablespace: 
--

CREATE TABLE twitter_api_twitterprofile (
    id integer NOT NULL,
    user_id integer NOT NULL,
    profile_image character varying(100) NOT NULL
);


ALTER TABLE public.twitter_api_twitterprofile OWNER TO bugle;

--
-- Name: twitter_api_twitterprofile_id_seq; Type: SEQUENCE; Schema: public; Owner: bugle
--

CREATE SEQUENCE twitter_api_twitterprofile_id_seq
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.twitter_api_twitterprofile_id_seq OWNER TO bugle;

--
-- Name: twitter_api_twitterprofile_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: bugle
--

ALTER SEQUENCE twitter_api_twitterprofile_id_seq OWNED BY twitter_api_twitterprofile.id;


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_group ALTER COLUMN id SET DEFAULT nextval('auth_group_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_group_permissions ALTER COLUMN id SET DEFAULT nextval('auth_group_permissions_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_message ALTER COLUMN id SET DEFAULT nextval('auth_message_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_permission ALTER COLUMN id SET DEFAULT nextval('auth_permission_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_user ALTER COLUMN id SET DEFAULT nextval('auth_user_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_user_groups ALTER COLUMN id SET DEFAULT nextval('auth_user_groups_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_user_user_permissions ALTER COLUMN id SET DEFAULT nextval('auth_user_user_permissions_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_blast ALTER COLUMN id SET DEFAULT nextval('bugle_blast_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_blast_favourited_by ALTER COLUMN id SET DEFAULT nextval('bugle_blast_favourited_by_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_blast_mentioned_users ALTER COLUMN id SET DEFAULT nextval('bugle_blast_mentioned_users_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_imageupload ALTER COLUMN id SET DEFAULT nextval('bugle_imageupload_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY django_admin_log ALTER COLUMN id SET DEFAULT nextval('django_admin_log_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY django_content_type ALTER COLUMN id SET DEFAULT nextval('django_content_type_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY django_site ALTER COLUMN id SET DEFAULT nextval('django_site_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY south_migrationhistory ALTER COLUMN id SET DEFAULT nextval('south_migrationhistory_id_seq'::regclass);


--
-- Name: id; Type: DEFAULT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY twitter_api_twitterprofile ALTER COLUMN id SET DEFAULT nextval('twitter_api_twitterprofile_id_seq'::regclass);


--
-- Name: auth_group_name_key; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_group
    ADD CONSTRAINT auth_group_name_key UNIQUE (name);


--
-- Name: auth_group_permissions_group_id_permission_id_key; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_group_permissions
    ADD CONSTRAINT auth_group_permissions_group_id_permission_id_key UNIQUE (group_id, permission_id);


--
-- Name: auth_group_permissions_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_group_permissions
    ADD CONSTRAINT auth_group_permissions_pkey PRIMARY KEY (id);


--
-- Name: auth_group_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_group
    ADD CONSTRAINT auth_group_pkey PRIMARY KEY (id);


--
-- Name: auth_message_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_message
    ADD CONSTRAINT auth_message_pkey PRIMARY KEY (id);


--
-- Name: auth_permission_content_type_id_codename_key; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_permission
    ADD CONSTRAINT auth_permission_content_type_id_codename_key UNIQUE (content_type_id, codename);


--
-- Name: auth_permission_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_permission
    ADD CONSTRAINT auth_permission_pkey PRIMARY KEY (id);


--
-- Name: auth_user_groups_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_user_groups
    ADD CONSTRAINT auth_user_groups_pkey PRIMARY KEY (id);


--
-- Name: auth_user_groups_user_id_group_id_key; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_user_groups
    ADD CONSTRAINT auth_user_groups_user_id_group_id_key UNIQUE (user_id, group_id);


--
-- Name: auth_user_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_user
    ADD CONSTRAINT auth_user_pkey PRIMARY KEY (id);


--
-- Name: auth_user_user_permissions_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_user_user_permissions
    ADD CONSTRAINT auth_user_user_permissions_pkey PRIMARY KEY (id);


--
-- Name: auth_user_user_permissions_user_id_permission_id_key; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_user_user_permissions
    ADD CONSTRAINT auth_user_user_permissions_user_id_permission_id_key UNIQUE (user_id, permission_id);


--
-- Name: auth_user_username_key; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY auth_user
    ADD CONSTRAINT auth_user_username_key UNIQUE (username);


--
-- Name: bugle_blast_favourited_by_blast_id_66451968463f31b3_uniq; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY bugle_blast_favourited_by
    ADD CONSTRAINT bugle_blast_favourited_by_blast_id_66451968463f31b3_uniq UNIQUE (blast_id, user_id);


--
-- Name: bugle_blast_favourited_by_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY bugle_blast_favourited_by
    ADD CONSTRAINT bugle_blast_favourited_by_pkey PRIMARY KEY (id);


--
-- Name: bugle_blast_mentioned_users_blast_id_504f2b6d98531d3a_uniq; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY bugle_blast_mentioned_users
    ADD CONSTRAINT bugle_blast_mentioned_users_blast_id_504f2b6d98531d3a_uniq UNIQUE (blast_id, user_id);


--
-- Name: bugle_blast_mentioned_users_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY bugle_blast_mentioned_users
    ADD CONSTRAINT bugle_blast_mentioned_users_pkey PRIMARY KEY (id);


--
-- Name: bugle_blast_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY bugle_blast
    ADD CONSTRAINT bugle_blast_pkey PRIMARY KEY (id);


--
-- Name: bugle_imageupload_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY bugle_imageupload
    ADD CONSTRAINT bugle_imageupload_pkey PRIMARY KEY (id);


--
-- Name: django_admin_log_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY django_admin_log
    ADD CONSTRAINT django_admin_log_pkey PRIMARY KEY (id);


--
-- Name: django_content_type_app_label_model_key; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY django_content_type
    ADD CONSTRAINT django_content_type_app_label_model_key UNIQUE (app_label, model);


--
-- Name: django_content_type_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY django_content_type
    ADD CONSTRAINT django_content_type_pkey PRIMARY KEY (id);


--
-- Name: django_session_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY django_session
    ADD CONSTRAINT django_session_pkey PRIMARY KEY (session_key);


--
-- Name: django_site_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY django_site
    ADD CONSTRAINT django_site_pkey PRIMARY KEY (id);


--
-- Name: south_migrationhistory_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY south_migrationhistory
    ADD CONSTRAINT south_migrationhistory_pkey PRIMARY KEY (id);


--
-- Name: twitter_api_twitterprofile_pkey; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY twitter_api_twitterprofile
    ADD CONSTRAINT twitter_api_twitterprofile_pkey PRIMARY KEY (id);


--
-- Name: twitter_api_twitterprofile_user_id_key; Type: CONSTRAINT; Schema: public; Owner: bugle; Tablespace: 
--

ALTER TABLE ONLY twitter_api_twitterprofile
    ADD CONSTRAINT twitter_api_twitterprofile_user_id_key UNIQUE (user_id);


--
-- Name: auth_group_permissions_group_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX auth_group_permissions_group_id ON auth_group_permissions USING btree (group_id);


--
-- Name: auth_group_permissions_permission_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX auth_group_permissions_permission_id ON auth_group_permissions USING btree (permission_id);


--
-- Name: auth_message_user_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX auth_message_user_id ON auth_message USING btree (user_id);


--
-- Name: auth_permission_content_type_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX auth_permission_content_type_id ON auth_permission USING btree (content_type_id);


--
-- Name: auth_user_groups_group_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX auth_user_groups_group_id ON auth_user_groups USING btree (group_id);


--
-- Name: auth_user_groups_user_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX auth_user_groups_user_id ON auth_user_groups USING btree (user_id);


--
-- Name: auth_user_user_permissions_permission_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX auth_user_user_permissions_permission_id ON auth_user_user_permissions USING btree (permission_id);


--
-- Name: auth_user_user_permissions_user_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX auth_user_user_permissions_user_id ON auth_user_user_permissions USING btree (user_id);


--
-- Name: bugle_blast_favourited_by_blast_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX bugle_blast_favourited_by_blast_id ON bugle_blast_favourited_by USING btree (blast_id);


--
-- Name: bugle_blast_favourited_by_user_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX bugle_blast_favourited_by_user_id ON bugle_blast_favourited_by USING btree (user_id);


--
-- Name: bugle_blast_in_reply_to_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX bugle_blast_in_reply_to_id ON bugle_blast USING btree (in_reply_to_id);


--
-- Name: bugle_blast_mentioned_users_blast_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX bugle_blast_mentioned_users_blast_id ON bugle_blast_mentioned_users USING btree (blast_id);


--
-- Name: bugle_blast_mentioned_users_user_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX bugle_blast_mentioned_users_user_id ON bugle_blast_mentioned_users USING btree (user_id);


--
-- Name: bugle_blast_user_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX bugle_blast_user_id ON bugle_blast USING btree (user_id);


--
-- Name: bugle_imageupload_user_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX bugle_imageupload_user_id ON bugle_imageupload USING btree (user_id);


--
-- Name: django_admin_log_content_type_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX django_admin_log_content_type_id ON django_admin_log USING btree (content_type_id);


--
-- Name: django_admin_log_user_id; Type: INDEX; Schema: public; Owner: bugle; Tablespace: 
--

CREATE INDEX django_admin_log_user_id ON django_admin_log USING btree (user_id);


--
-- Name: auth_group_permissions_permission_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_group_permissions
    ADD CONSTRAINT auth_group_permissions_permission_id_fkey FOREIGN KEY (permission_id) REFERENCES auth_permission(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: auth_message_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_message
    ADD CONSTRAINT auth_message_user_id_fkey FOREIGN KEY (user_id) REFERENCES auth_user(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: auth_user_groups_group_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_user_groups
    ADD CONSTRAINT auth_user_groups_group_id_fkey FOREIGN KEY (group_id) REFERENCES auth_group(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: auth_user_user_permissions_permission_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_user_user_permissions
    ADD CONSTRAINT auth_user_user_permissions_permission_id_fkey FOREIGN KEY (permission_id) REFERENCES auth_permission(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: blast_id_refs_id_15d749b6340388d0; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_blast_mentioned_users
    ADD CONSTRAINT blast_id_refs_id_15d749b6340388d0 FOREIGN KEY (blast_id) REFERENCES bugle_blast(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: blast_id_refs_id_42cebf986f1f95cf; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_blast_favourited_by
    ADD CONSTRAINT blast_id_refs_id_42cebf986f1f95cf FOREIGN KEY (blast_id) REFERENCES bugle_blast(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: content_type_id_refs_id_728de91f; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_permission
    ADD CONSTRAINT content_type_id_refs_id_728de91f FOREIGN KEY (content_type_id) REFERENCES django_content_type(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: django_admin_log_content_type_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY django_admin_log
    ADD CONSTRAINT django_admin_log_content_type_id_fkey FOREIGN KEY (content_type_id) REFERENCES django_content_type(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: django_admin_log_user_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY django_admin_log
    ADD CONSTRAINT django_admin_log_user_id_fkey FOREIGN KEY (user_id) REFERENCES auth_user(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: group_id_refs_id_3cea63fe; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_group_permissions
    ADD CONSTRAINT group_id_refs_id_3cea63fe FOREIGN KEY (group_id) REFERENCES auth_group(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: in_reply_to_id_refs_id_2306e1c18c7e71b3; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_blast
    ADD CONSTRAINT in_reply_to_id_refs_id_2306e1c18c7e71b3 FOREIGN KEY (in_reply_to_id) REFERENCES bugle_blast(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: user_id_refs_id_3a4f361cf4030473; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_blast_favourited_by
    ADD CONSTRAINT user_id_refs_id_3a4f361cf4030473 FOREIGN KEY (user_id) REFERENCES auth_user(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: user_id_refs_id_46455c67a6f5690f; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_blast
    ADD CONSTRAINT user_id_refs_id_46455c67a6f5690f FOREIGN KEY (user_id) REFERENCES auth_user(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: user_id_refs_id_4a0ff6a3848ddb3; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_imageupload
    ADD CONSTRAINT user_id_refs_id_4a0ff6a3848ddb3 FOREIGN KEY (user_id) REFERENCES auth_user(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: user_id_refs_id_4b0a55c4b5a35086; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY bugle_blast_mentioned_users
    ADD CONSTRAINT user_id_refs_id_4b0a55c4b5a35086 FOREIGN KEY (user_id) REFERENCES auth_user(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: user_id_refs_id_546dc2c433ee4505; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY twitter_api_twitterprofile
    ADD CONSTRAINT user_id_refs_id_546dc2c433ee4505 FOREIGN KEY (user_id) REFERENCES auth_user(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: user_id_refs_id_831107f1; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_user_groups
    ADD CONSTRAINT user_id_refs_id_831107f1 FOREIGN KEY (user_id) REFERENCES auth_user(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: user_id_refs_id_f2045483; Type: FK CONSTRAINT; Schema: public; Owner: bugle
--

ALTER TABLE ONLY auth_user_user_permissions
    ADD CONSTRAINT user_id_refs_id_f2045483 FOREIGN KEY (user_id) REFERENCES auth_user(id) DEFERRABLE INITIALLY DEFERRED;


--
-- Name: public; Type: ACL; Schema: -; Owner: postgres
--

REVOKE ALL ON SCHEMA public FROM PUBLIC;
REVOKE ALL ON SCHEMA public FROM postgres;
GRANT ALL ON SCHEMA public TO postgres;
GRANT ALL ON SCHEMA public TO PUBLIC;


--
-- PostgreSQL database dump complete
--

